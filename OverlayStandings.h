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

#include <assert.h>
#include "Overlay.h"
#include "Config.h"
#include "OverlayDebug.h"

class OverlayStandings : public Overlay
{
public:

    const float DefaultFontSize = 15;

    enum class Columns { POSITION, CAR_NUMBER, NAME, DELTA, BEST, LAST, LICENSE, IRATING, PIT };

    OverlayStandings()
        : Overlay("OverlayStandings")
    {}

protected:

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
        m_columns.add( (int)Columns::PIT,        computeTextExtent( L"P.Age", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
        m_columns.add( (int)Columns::LICENSE,    computeTextExtent( L"A 4.44", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x, fontSize/6 );
        m_columns.add( (int)Columns::IRATING,    computeTextExtent( L"999.9k", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x, fontSize/6 );
        m_columns.add( (int)Columns::BEST,       computeTextExtent( L"999.99.999", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
        m_columns.add( (int)Columns::LAST,       computeTextExtent( L"999.99.999", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
        m_columns.add( (int)Columns::DELTA,      computeTextExtent( L"9999.9999", m_dwriteFactory.Get(), m_textFormat.Get() ).x, fontSize/2 );
    }

    virtual void onUpdate()
    {
        struct CarInfo {
            int     carIdx = 0;
            int     lapCount = 0;
            float   pctAroundLap = 0;
            int     lapDelta = 0;
            float   delta = 0;
            int     position = 0;
            float   best = 0;
            float   last = 0;
            bool    hasFastestLap = false;
            int     pitAge = 0;
        };
        std::vector<CarInfo> carInfo;
        carInfo.reserve( IR_MAX_CARS );

        // Init array
        float fastestLapTime = FLT_MAX;
        int fastestLapIdx = -1;
        for( int i=0; i<IR_MAX_CARS; ++i )
        {
            const Car& car = ir_session.cars[i];

            if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                continue;

            CarInfo ci;
            ci.carIdx       = i;
            ci.lapCount     = std::max( ir_CarIdxLap.getInt(i), ir_CarIdxLapCompleted.getInt(i) );
            ci.position     = ir_getPosition(i);
            ci.pctAroundLap = ir_CarIdxLapDistPct.getFloat(i);
            ci.delta        = ir_session.sessionType!=SessionType::RACE ? 0 : -ir_CarIdxF2Time.getFloat(i);
            ci.last         = ir_CarIdxLastLapTime.getFloat(i);
            ci.pitAge       = ir_CarIdxLap.getInt(i) - car.lastLapInPits;

            ci.best         = ir_CarIdxBestLapTime.getFloat(i);
            if( ir_session.sessionType==SessionType::RACE && ir_SessionState.getInt()<=irsdk_StateWarmup || ir_session.sessionType==SessionType::QUALIFY && ci.best<=0 )
                ci.best = car.qualTime;

            carInfo.push_back( ci );

            if( ci.best > 0 && ci.best < fastestLapTime ) {
                fastestLapTime = ci.best;
                fastestLapIdx = (int)carInfo.size()-1;
            }
        }

        if( fastestLapIdx >= 0 )
            carInfo[fastestLapIdx].hasFastestLap = true;

        // Sort by position
        std::sort( carInfo.begin(), carInfo.end(),
            []( const CarInfo& a, const CarInfo& b ) {
                const int ap = a.position<=0 ? INT_MAX : a.position;
                const int bp = b.position<=0 ? INT_MAX : b.position;
                return ap < bp;
            } );

        // Compute lap deltas to leader
        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const CarInfo& ciLeader = carInfo[0];
            CarInfo&       ci       = carInfo[i];
            ci.lapDelta = ir_getLapDeltaToLeader( ci.carIdx, ciLeader.carIdx );
        }

        const float  fontSize           = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
        const float  lineSpacing        = g_cfg.getFloat( m_name, "line_spacing", 8 );
        const float  lineHeight         = fontSize + lineSpacing;
        const float4 selfCol            = g_cfg.getFloat4( m_name, "self_col", float4(0.94f,0.67f,0.13f,1) );
        const float4 buddyCol           = g_cfg.getFloat4( m_name, "buddy_col", float4(0.2f,0.75f,0,1) );
        const float4 flaggedCol         = g_cfg.getFloat4( m_name, "flagged_col", float4(0.68f,0.42f,0.2f,1) );
        const float4 otherCarCol        = g_cfg.getFloat4( m_name, "other_car_col", float4(1,1,1,0.9f) );
        const float4 headerCol          = g_cfg.getFloat4( m_name, "header_col", float4(0.7f,0.7f,0.7f,0.9f) );
        const float4 carNumberTextCol   = g_cfg.getFloat4( m_name, "car_number_text_col", float4(0,0,0,0.9f) );
        const float4 alternateLineBgCol = g_cfg.getFloat4( m_name, "alternate_line_background_col", float4(0.5f,0.5f,0.5f,0.1f) );
        const float4 iratingTextCol     = g_cfg.getFloat4( m_name, "irating_text_col", float4(0,0,0,0.9f) );
        const float4 iratingBgCol       = g_cfg.getFloat4( m_name, "irating_background_col", float4(1,1,1,0.85f) );
        const float4 licenseTextCol     = g_cfg.getFloat4( m_name, "license_text_col", float4(1,1,1,0.9f) );
        const float4 fastestLapCol      = g_cfg.getFloat4( m_name, "fastest_lap_col", float4(1,0,1,1) );
        const float4 pitCol             = g_cfg.getFloat4( m_name, "pit_col", float4(0.94f,0.8f,0.13f,1) );
        const float  licenseBgAlpha     = g_cfg.getFloat( m_name, "license_background_alpha", 0.8f );
        const bool   imperial           = ir_DisplayUnits.getInt() == 0;

        const float xoff = 10.0f;
        const float yoff = 10;
        m_columns.layout( (float)m_width - 2*xoff );
        float y = yoff + lineHeight/2;
        const float ybottom = m_height - lineHeight * 1.5f;

        const ColumnLayout::Column* clm = nullptr;
        wchar_t s[512];
        std::string str;
        D2D1_RECT_F r = {};
        D2D1_ROUNDED_RECT rr = {};

        m_renderTarget->BeginDraw();
        m_brush->SetColor( headerCol );

        // Headers
        clm = m_columns.get( (int)Columns::POSITION );
        swprintf( s, _countof(s), L"Pos." );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

        clm = m_columns.get( (int)Columns::CAR_NUMBER );
        swprintf( s, _countof(s), L"No." );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

        clm = m_columns.get( (int)Columns::NAME );
        swprintf( s, _countof(s), L"Driver" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );

        clm = m_columns.get( (int)Columns::PIT );
        swprintf( s, _countof(s), L"P.Age" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

        clm = m_columns.get( (int)Columns::LICENSE );
        swprintf( s, _countof(s), L"SR" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

        clm = m_columns.get( (int)Columns::IRATING );
        swprintf( s, _countof(s), L"IR" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

        clm = m_columns.get( (int)Columns::BEST );
        swprintf( s, _countof(s), L"Best" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );

        clm = m_columns.get( (int)Columns::LAST );
        swprintf( s, _countof(s), L"Last" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );

        clm = m_columns.get( (int)Columns::DELTA );
        swprintf( s, _countof(s), L"Delta" );
        m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );

        // Content
        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            y = 2*yoff + lineHeight/2 + (i+1)*lineHeight;

            if( y+lineHeight/2 > ybottom )
                break;

            // Alternating line backgrounds
            if( i & 1 && alternateLineBgCol.a > 0 )
            {
                D2D1_RECT_F r = { 0, y-lineHeight/2, (float)m_width,  y+lineHeight/2 };
                m_brush->SetColor( alternateLineBgCol );
                m_renderTarget->FillRectangle( &r, m_brush.Get() );
            }

            const CarInfo&  ci  = carInfo[i];
            const Car&      car = ir_session.cars[ci.carIdx];

            // Dim color if player is disconnected.
            // TODO: this isn't 100% accurate, I think, because a car might be "not in world" while the player
            // is still connected? I haven't been able to find a better way to do this, though.
            const bool isGone = !car.isSelf && ir_CarIdxTrackSurface.getInt(ci.carIdx) == irsdk_NotInWorld;
            float4 textCol = car.isSelf ? selfCol : (car.isBuddy ? buddyCol : (car.isFlagged?flaggedCol:otherCarCol));
            if( isGone )
                textCol.a *= 0.5f;

            // Position
            if( ci.position > 0 )
            {
                clm = m_columns.get( (int)Columns::POSITION );
                m_brush->SetColor( textCol );
                swprintf( s, _countof(s), L"P%d", ci.position );
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
                m_brush->SetColor( textCol );
                m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                m_brush->SetColor( carNumberTextCol );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Name
            {
                clm = m_columns.get( (int)Columns::NAME );
                m_brush->SetColor( textCol );
                swprintf( s, _countof(s), L"%S", car.userName.c_str() );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
            }

            // Pit age
            if( !ir_isPreStart() && (ci.pitAge>=0||ir_CarIdxOnPitRoad.getBool(ci.carIdx)) )
            {
                clm = m_columns.get( (int)Columns::PIT );
                m_brush->SetColor( pitCol );
                swprintf( s, _countof(s), L"%d", ci.pitAge );
                r = { xoff+clm->textL, y-lineHeight/2+2, xoff+clm->textR, y+lineHeight/2-2 };
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

            // License/SR
            {
                clm = m_columns.get( (int)Columns::LICENSE );
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
            {
                clm = m_columns.get( (int)Columns::IRATING );
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

            // Best
            {
                clm = m_columns.get( (int)Columns::BEST );
                str.clear();
                if( ci.best > 0 )
                    str = formatLaptime( ci.best );
                m_brush->SetColor( ci.hasFastestLap ? fastestLapCol : otherCarCol );
                m_text.render( m_renderTarget.Get(), toWide(str).c_str(), m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
            }

            // Last
            {
                clm = m_columns.get( (int)Columns::LAST );
                str.clear();
                if( ci.last > 0 )
                    str = formatLaptime( ci.last );
                m_brush->SetColor( otherCarCol );
                m_text.render( m_renderTarget.Get(), toWide(str).c_str(), m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
            }

            // Delta
            if( ci.lapDelta || ci.delta )
            {
                clm = m_columns.get( (int)Columns::DELTA );
                if( ci.lapDelta < 0 )
                    swprintf( s, _countof(s), L"%d L", ci.lapDelta );
                else
                    swprintf( s, _countof(s), L"%.03f", ci.delta );
                m_brush->SetColor( otherCarCol );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff+clm->textL, xoff+clm->textR, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
            }
        }
        
        // Footer
        {
            float trackTemp = ir_TrackTempCrew.getFloat();
            float airTemp   = ir_AirTemp.getFloat();
            char  tempUnit  = 'C';

            if( imperial ) {
                trackTemp = celsiusToFahrenheit( trackTemp );
                airTemp   = celsiusToFahrenheit( airTemp );
                tempUnit  = 'F';
            }

            m_brush->SetColor(float4(1,1,1,0.4f));
            m_renderTarget->DrawLine( float2(0,ybottom),float2((float)m_width,ybottom),m_brush.Get() );
            swprintf( s, _countof(s), L"SoF: %d      Track Temp: %.1f°%c      Air Temp: %.1f°%c      Setup: %s      Subsession: %d", ir_session.sof, trackTemp, tempUnit, airTemp, tempUnit, ir_session.isFixedSetup?L"fixed":L"open", ir_session.subsessionId );
            y = m_height - (m_height-ybottom)/2;
            m_brush->SetColor( headerCol );
            m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), xoff, (float)m_width-2*xoff, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
        }

        m_renderTarget->EndDraw();
    }

    virtual bool canEnableWhileNotDriving() const
    {
        return true;
    }

protected:

    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatSmall;

    ColumnLayout m_columns;
    TextCache    m_text;
};
