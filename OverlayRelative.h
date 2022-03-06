/*
MIT License

Copyright (c) 2021-2022 L. E. Spalt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>
#include <algorithm>
#include "Overlay.h"
#include "iracing.h"
#include "Config.h"

class OverlayRelative : public Overlay
{
    public:

        const float DefaultFontSize = 15.3f;

        OverlayRelative()
            : Overlay("OverlayRelative")
        {}

    protected:

        enum class Columns { POSITION, CAR_NUMBER, NAME, DELTA, LICENSE, SAFETY_RATING, IRATING, PIT };

        virtual void onEnable()
        {
            onConfigChanged();  // trigger font load
        }

        virtual void onDisable()
        {
            m_text.reset();
        }

        virtual void onConfigChanged()
        {
            m_text.reset( m_dwriteFactory.Get() );

            const std::string font = g_cfg.getString( m_name, "font", "Microsoft YaHei UI" );
            const float fontSize = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const int fontWeight = g_cfg.getInt( m_name, "font_weight", 500 );
            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*0.8f, L"en-us", &m_textFormatSmall ));
            m_textFormatSmall->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatSmall->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            // Determine widths of text columns
            m_columns.reset();
            m_columns.add( (int)Columns::POSITION,   computeTextExtent( L"P99", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
            m_columns.add( (int)Columns::CAR_NUMBER, computeTextExtent( L"#999", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
            m_columns.add( (int)Columns::NAME,       0, fontSize/2 );
            m_columns.add( (int)Columns::DELTA,      computeTextExtent( L"+99L  -99.9", m_dwriteFactory.Get(), m_textFormat.Get() ).x, 1, fontSize/2 );

            if( g_cfg.getBool(m_name,"show_pit_age",true) )
                m_columns.add( (int)Columns::PIT,           computeTextExtent( L"999", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x, fontSize/4 );
            if( g_cfg.getBool(m_name,"show_license",true) && !g_cfg.getBool(m_name,"show_sr",false) )
                m_columns.add( (int)Columns::LICENSE,       computeTextExtent( L" A ", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x*1.6f, fontSize/10 );
            if( g_cfg.getBool(m_name,"show_sr",false) )
                m_columns.add( (int)Columns::SAFETY_RATING, computeTextExtent( L"A 4.44", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x, fontSize/8 );
            if( g_cfg.getBool(m_name,"show_irating",true) )
                m_columns.add( (int)Columns::IRATING,       computeTextExtent( L"999.9k", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x, fontSize/8 );
        }

        virtual void onUpdate()
        {            
            struct CarInfo {
                int     carIdx = 0;
                float   delta = 0;
                int     lapDelta = 0;
                int     pitAge = 0;
            };
            std::vector<CarInfo> relatives;
            relatives.reserve( IR_MAX_CARS );

            // Populate cars with the ones for which a relative/delta comparison is valid
            for( int i=0; i<IR_MAX_CARS; ++i )
            {
                const Car& car = ir_session.cars[i];

                const int lapcountS = ir_CarIdxLap.getInt(ir_session.driverCarIdx);
                const int lapcountC = ir_CarIdxLap.getInt(i);

                if( lapcountC >= 0 && !car.isSpectator && car.carNumber>=0 )
                {
                    // Add the pace car only under yellow or initial pace lap
                    if( car.isPaceCar && !(ir_SessionFlags.getInt() & (irsdk_caution|irsdk_cautionWaving)) && !ir_isPreStart() )
                        continue;

                    // If the other car is up to half a lap in front, we consider the delta 'ahead', otherwise 'behind'.

                    float delta = 0;
                    int   lapDelta = lapcountC - lapcountS;

                    const float L = ir_estimateLaptime();
                    const float C = ir_CarIdxEstTime.getFloat(i);
                    const float S = ir_CarIdxEstTime.getFloat(ir_session.driverCarIdx);

                    // Does the delta between us and the other car span across the start/finish line?
                    const bool wrap = fabsf(ir_CarIdxLapDistPct.getFloat(i) - ir_CarIdxLapDistPct.getFloat(ir_session.driverCarIdx)) > 0.5f;

                    if( wrap )
                    {
                        delta     = S > C ? (C-S)+L : (C-S)-L;
                        lapDelta += S > C ? -1 : 1;
                    }
                    else
                    {
                        delta = C - S;
                    }

                    // Assume no lap delta when not in a race, because we don't want to show drivers as lapped/lapping there.
                    // Also reset it during initial pacing, since iRacing for some reason starts counting
                    // during the pace lap but then resets the counter a couple seconds in, confusing the logic.
                    // And consider the pace car in the same lap as us, too.
                    if( ir_session.sessionType!=SessionType::RACE || ir_isPreStart() || car.isPaceCar )
                    {
                        lapDelta = 0;
                    }

                    CarInfo ci;
                    ci.carIdx = i;
                    ci.delta = delta;
                    ci.lapDelta = lapDelta;
                    ci.pitAge = ir_CarIdxLap.getInt(i) - car.lastLapInPits;
                    relatives.push_back( ci );
                }
            }

            // Sort by delta
            std::sort( relatives.begin(), relatives.end(), 
                []( const CarInfo& a, const CarInfo&b ) { return a.delta > b.delta; } );

            // Locate our driver's index in the new array
            int selfCarInfoIdx = -1;
            for( int i=0; i<(int)relatives.size(); ++i )
            {
                if( relatives[i].carIdx == ir_session.driverCarIdx ) {
                    selfCarInfoIdx = i;
                    break;
                }
            }

            // Something's wrong if we didn't find our driver. Bail.
            if( selfCarInfoIdx < 0 )
                return;

            // Display such that our driver is in the vertical center of the area where we're listing cars

            const float  fontSize           = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const float  lineSpacing        = g_cfg.getFloat( m_name, "line_spacing", 6 );
            const float  lineHeight         = fontSize + lineSpacing;
            const float4 selfCol            = g_cfg.getFloat4( m_name, "self_col", float4(0.94f,0.67f,0.13f,1) );
            const float4 sameLapCol         = g_cfg.getFloat4( m_name, "same_lap_col", float4(1,1,1,1) );
            const float4 lapAheadCol        = g_cfg.getFloat4( m_name, "lap_ahead_col", float4(0.9f,0.17f,0.17f,1) );
            const float4 lapBehindCol       = g_cfg.getFloat4( m_name, "lap_behind_col", float4(0,0.71f,0.95f,1) );
            const float4 iratingTextCol     = g_cfg.getFloat4( m_name, "irating_text_col", float4(0,0,0,0.9f) );
            const float4 iratingBgCol       = g_cfg.getFloat4( m_name, "irating_background_col", float4(1,1,1,0.85f) );
            const float4 licenseTextCol     = g_cfg.getFloat4( m_name, "license_text_col", float4(1,1,1,0.9f) );
            const float  licenseBgAlpha     = g_cfg.getFloat( m_name, "license_background_alpha", 0.8f );
            const float4 alternateLineBgCol = g_cfg.getFloat4( m_name, "alternate_line_background_col", float4(0.5f,0.5f,0.5f,0) );
            const float4 buddyCol           = g_cfg.getFloat4( m_name, "buddy_col", float4(0.2f,0.75f,0,1) );
            const float4 flaggedCol         = g_cfg.getFloat4( m_name, "flagged_col", float4(0.6f,0.35f,0.2f,1) );
            const float4 carNumberBgCol     = g_cfg.getFloat4( m_name, "car_number_background_col", float4(1,1,1,0.9f) );
            const float4 carNumberTextCol   = g_cfg.getFloat4( m_name, "car_number_text_col", float4(0,0,0,0.9f) );
            const float4 pitCol             = g_cfg.getFloat4( m_name, "pit_col", float4(0.94f,0.8f,0.13f,1) );
            const bool   minimapEnabled     = g_cfg.getBool( m_name, "minimap_enabled", true );
            const bool   minimapIsRelative  = g_cfg.getBool( m_name, "minimap_is_relative", true );
            const float4 minimapBgCol       = g_cfg.getFloat4( m_name, "minimap_background_col", float4(0,0,0,0.13f) );
            const float  listingAreaTop     = minimapEnabled ? 30 : 10.0f;
            const float  listingAreaBot     = m_height - 10.0f;
            const float  yself              = listingAreaTop + (listingAreaBot-listingAreaTop) / 2.0f;
            const int    entriesAbove       = int( (yself - lineHeight/2 - listingAreaTop) / lineHeight );

            float y = yself - entriesAbove * lineHeight;

            const float xoff = 10.0f;
            m_columns.layout( (float)m_width - 20 );

            m_renderTarget->BeginDraw();
            for( int cnt=0, i=selfCarInfoIdx-entriesAbove; i<(int)relatives.size() && y<=listingAreaBot-lineHeight/2; ++i, y+=lineHeight, ++cnt )
            {
                // Alternating line backgrounds
                if( cnt & 1 && alternateLineBgCol.a > 0 )
                {
                    D2D1_RECT_F r = { 0, y-lineHeight/2, (float)m_width,  y+lineHeight/2 };
                    m_brush->SetColor( alternateLineBgCol );
                    m_renderTarget->FillRectangle( &r, m_brush.Get() );
                }

                // Skip if we don't have a car to list for this line
                if( i < 0 )
                    continue;

                const CarInfo& ci  = relatives[i];
                const Car&     car = ir_session.cars[ci.carIdx];

                // Determine text color
                float4 col = sameLapCol;
                if( ci.lapDelta > 0 )
                    col = lapAheadCol;
                if( ci.lapDelta < 0 )
                    col = lapBehindCol;

                if( car.isSelf )
                    col = selfCol;
                else if( ir_CarIdxOnPitRoad.getBool(ci.carIdx) )
                    col.a *= 0.5f;
                
                wchar_t s[512];
                D2D1_RECT_F r = {};
                D2D1_ROUNDED_RECT rr = {};
                const ColumnLayout::Column* clm = nullptr;
                
                // Position
                if( ir_getPosition(ci.carIdx) > 0 )
                {
                    clm = m_columns.get( (int)Columns::POSITION );
                    m_brush->SetColor( col );
                    swprintf( s, _countof(s), L"P%d", ir_getPosition(ci.carIdx) );
                    m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                }

                // Car number
                {
                    clm = m_columns.get( (int)Columns::CAR_NUMBER );
                    swprintf( s, _countof(s), L"#%S", car.carNumberStr.c_str() );
                    r = { xoff+clm->textL, y-lineHeight/2, xoff+clm->textR, y+lineHeight/2 };
                    rr.rect = { r.left-2, r.top+1, r.right+2, r.bottom-1 };
                    rr.radiusX = 3;
                    rr.radiusY = 3;
                    m_brush->SetColor( car.isSelf ? selfCol : (car.isBuddy ? buddyCol : (car.isFlagged?flaggedCol:carNumberBgCol)) );
                    m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                    m_brush->SetColor( carNumberTextCol );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }

                // Name
                {
                    clm = m_columns.get( (int)Columns::NAME );
                    swprintf( s, _countof(s), L"%S", car.userName.c_str() );
                    m_brush->SetColor( col );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
                }

                // Delta
                {
                    clm = m_columns.get( (int)Columns::DELTA );
                    if( ci.lapDelta )
                        swprintf( s, _countof(s), L"%+dL  %.1f", ci.lapDelta, ci.delta );
                    else
                        swprintf( s, _countof(s), L"%.1f", ci.delta );
                    m_brush->SetColor( col );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                }

                // Pit age
                if( (clm = m_columns.get((int)Columns::PIT)) && !ir_isPreStart() && (ci.pitAge>=0||ir_CarIdxOnPitRoad.getBool(ci.carIdx)) )
                {
                    r = { xoff+clm->textL, y-lineHeight/2+2, xoff+clm->textR, y+lineHeight/2-2 };
                    m_brush->SetColor( pitCol );
                    m_renderTarget->DrawRectangle( &r, m_brush.Get() );
                    if( ir_CarIdxOnPitRoad.getBool(ci.carIdx) ) {
                        swprintf( s, _countof(s), L"PIT" );
                        m_renderTarget->FillRectangle( &r, m_brush.Get() );
                        m_brush->SetColor( float4(0,0,0,1) );
                    }
                    else {
                        swprintf( s, _countof(s), L"%d", ci.pitAge );
                        m_renderTarget->DrawRectangle( &r, m_brush.Get() );
                    }
                    m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }

                // License without SR
                if( clm = m_columns.get( (int)Columns::LICENSE ) )
                {
                    swprintf( s, _countof(s), L"%C", car.licenseChar );
                    r = { xoff+clm->textL, y-lineHeight/2, xoff+clm->textR, y+lineHeight/2 };
                    rr.rect = { r.left+1, r.top+1, r.right-1, r.bottom-1 };
                    rr.radiusX = 3;
                    rr.radiusY = 3;
                    float4 c = car.licenseCol;
                    c.a = licenseBgAlpha;
                    m_brush->SetColor( c );
                    m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                    m_brush->SetColor( licenseTextCol );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }

                // License with SR
                if( clm = m_columns.get( (int)Columns::SAFETY_RATING ) )
                {
                    swprintf( s, _countof(s), L"%C %.1f", car.licenseChar, car.licenseSR );
                    r = { xoff+clm->textL, y-lineHeight/2, xoff+clm->textR, y+lineHeight/2 };
                    rr.rect = { r.left+1, r.top+1, r.right-1, r.bottom-1 };
                    rr.radiusX = 3;
                    rr.radiusY = 3;
                    float4 c = car.licenseCol;
                    c.a = licenseBgAlpha;
                    m_brush->SetColor( c );
                    m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                    m_brush->SetColor( licenseTextCol );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }

                // Irating
                if( clm = m_columns.get( (int)Columns::IRATING ) )
                {
                    swprintf( s, _countof(s), L"%.1fk", (float)car.irating/1000.0f );
                    r = { xoff+clm->textL, y-lineHeight/2, xoff+clm->textR, y+lineHeight/2 };
                    rr.rect = { r.left+1, r.top+1, r.right-1, r.bottom-1 };
                    rr.radiusX = 3;
                    rr.radiusY = 3;
                    m_brush->SetColor( iratingBgCol );
                    m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                    m_brush->SetColor( iratingTextCol );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }
            }

            // Minimap
            if( minimapEnabled )
            {
                const float y = 10;
                const float x = 10;
                const float h = 15;
                const float w = (float)m_width - 2*x;
                D2D1_RECT_F r = { x, y, x+w, y+h };
                m_brush->SetColor( minimapBgCol );
                m_renderTarget->FillRectangle( &r, m_brush.Get() );                

                // phases: lap down, same lap, lap ahead, buddies, pacecar, self
                for( int phase=0; phase<6; ++phase )
                {
                    float4 baseCol = float4(0,0,0,0);
                    switch(phase)
                    {
                        case 0: baseCol = lapBehindCol; break;
                        case 1: baseCol = sameLapCol; break;
                        case 2: baseCol = lapAheadCol; break;
                        case 3: baseCol = buddyCol; break;
                        case 4: baseCol = float4(1,1,1,1); break;
                        case 5: baseCol = selfCol; break;
                        default: break;
                    }

                    for( int i=0; i<(int)relatives.size(); ++i )
                    {
                        const CarInfo& ci     = relatives[i];
                        const Car&     car    = ir_session.cars[ci.carIdx];

                        if( phase == 0 && ci.lapDelta >= 0 )
                            continue;
                        if( phase == 1 && ci.lapDelta != 0 )
                            continue;
                        if( phase == 2 && ci.lapDelta <= 0 )
                            continue;
                        if( phase == 3 && !car.isBuddy )
                            continue;
                        if( phase == 4 && !car.isPaceCar )
                            continue;
                        if( phase == 5 && !car.isSelf )
                            continue;
                        
                        float e = ir_CarIdxLapDistPct.getFloat(ci.carIdx);

                        const float eself = ir_CarIdxLapDistPct.getFloat(ir_session.driverCarIdx);

                        if( minimapIsRelative )
                        {
                            e = e - eself + 0.5f;
                            if( e > 1 )
                                e -= 1;
                            if( e < 0 )
                                e += 1;
                        }
                        e = e * w + x;

                        float4 col = baseCol;
                        if( !car.isSelf && ir_CarIdxOnPitRoad.getBool(ci.carIdx) )
                            col.a *= 0.5f;

                        const float dx = 2;
                        const float dy = car.isSelf || car.isPaceCar ? 4.0f : 0.0f;
                        r = {e-dx, y+2-dy, e+dx, y+h-2+dy};
                        m_brush->SetColor( col );
                        m_renderTarget->FillRectangle( &r, m_brush.Get() );
                    }
                }
            }
            m_renderTarget->EndDraw();
        }

    protected:

        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatSmall;

        ColumnLayout m_columns;
        TextCache    m_text;
};
