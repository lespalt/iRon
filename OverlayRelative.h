/*
MIT License

Copyright (c) 2021 lespalt

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

class OverlayRelative : public Overlay
{
    public:

        OverlayRelative()
            : Overlay("OverlayRelative")
        {}

    protected:

        virtual void onEnable()
        {
            onConfigChanged();  // trigger font load
        }

        virtual void onDisable()
        {}

        virtual void onConfigChanged()
        {
            const std::string font = g_cfg.getString( m_name, "font" );
            const float fontSize = g_cfg.getFloat( m_name, "font_size" );
            const int fontWeight = g_cfg.getInt( m_name, "font_weight" );
            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*0.8f, L"en-us", &m_textFormatSmall ));
            m_textFormatSmall->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatSmall->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            // Determine minimum widths of fixed-width columns
            m_posWidth     = computeTextExtent( L"P99", m_dwriteFactory.Get(), m_textFormat.Get() ).x;
            m_numWidth     = computeTextExtent( L"#99", m_dwriteFactory.Get(), m_textFormat.Get() ).x;
            m_deltaWidth   = computeTextExtent( L"999.9", m_dwriteFactory.Get(), m_textFormat.Get() ).x;
            m_iratingWidth = computeTextExtent( L"999.9k", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x;
            m_safetyWidth  = computeTextExtent( L"A 4.44", m_dwriteFactory.Get(), m_textFormatSmall.Get() ).x;
        }

        virtual void onUpdate()
        {
            struct CarInfo {
                int     carIdx = 0;
                float   delta = 0;
                int     lapDelta = 0;
            };
            std::vector<CarInfo> relatives;
            relatives.reserve( IR_MAX_CARS );

            // Populate cars with the ones for which a relative/delta comparison is valid
            for( int i=0; i<IR_MAX_CARS; ++i )
            {
                const Car& car = ir_session.cars[i];

                const int lapcountS = ir_CarIdxLap.getInt(ir_session.driverCarIdx);
                const int lapcountC = ir_CarIdxLap.getInt(i);

                if( lapcountC >= 0 && !car.isPaceCar && !car.isSpectator && car.carNumber>=0 )
                {
                    // If the other car is up to half a lap in front, we consider the delta 'ahead', otherwise 'behind'.

                    float delta = 0;
                    int   lapDelta = lapcountC - lapcountS;

                    const float L = ir_session.cars[i].carClassEstLapTime;  // TODO: is this good enough to estimate lap time for wrap-around computations?
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

                    CarInfo ci;
                    ci.carIdx = i;
                    ci.delta = delta;
                    ci.lapDelta = ir_session.sessionType==SessionType::PRACTICE ? 0 : lapDelta;
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

            const float  fontSize           = g_cfg.getFloat( m_name, "font_size" );
            const float  lineSpacing        = g_cfg.getFloat( m_name, "line_spacing" );
            const float  lineHeight         = fontSize + lineSpacing;
            const float4 selfCol            = g_cfg.getFloat4( m_name, "self_col" );
            const float4 sameLapCol         = g_cfg.getFloat4( m_name, "same_lap_col" );
            const float4 lapAheadCol        = g_cfg.getFloat4( m_name, "lap_ahead_col" );
            const float4 lapBehindCol       = g_cfg.getFloat4( m_name, "lap_behind_col" );
            const float4 iratingTextCol     = g_cfg.getFloat4( m_name, "irating_text_col" );
            const float4 iratingBgCol       = g_cfg.getFloat4( m_name, "irating_background_col" );
            const float4 licenseTextCol     = g_cfg.getFloat4( m_name, "license_text_col" );
            const float  licenseBgAlpha     = g_cfg.getFloat( m_name, "license_background_alpha" );
            const float4 alternateLineBgCol = g_cfg.getFloat4( m_name, "alternate_line_background_col" );
            const float4 buddyCol           = g_cfg.getFloat4( m_name, "buddy_col" );
            const float  listingAreaTop     = 10.0f;
            const float  listingAreaBot     = m_height - 10.0f;
            const float  yself              = listingAreaTop + (listingAreaBot-listingAreaTop) / 2.0f;
            const int    entriesAbove       = int( (yself - lineHeight/2 - listingAreaTop) / lineHeight );

            float y = yself - entriesAbove * lineHeight;

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

                if( i==selfCarInfoIdx )
                    col = selfCol;
                else if( ir_CarIdxOnPitRoad.getBool(ci.carIdx) )
                    col.a *= 0.5f;

                // Track fixed-width column offsets from left and right side so we can give a dynamically computed width to the name column
                float xl = 10.0f;
                float xr = m_width-10.0f;

                wchar_t s[512];
                D2D1_RECT_F r = {};
                D2D1_ROUNDED_RECT rr = {};
                
                // Position
                m_brush->SetColor( col );
                swprintf( s, sizeof(s), L"P%d", ir_CarIdxPosition.getInt(ci.carIdx) );
                r = { xl, y-lineHeight/2, xl+m_posWidth, y+lineHeight/2 };
                m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
                xl += m_posWidth * 1.5f;

                // Car number
                swprintf( s, sizeof(s), L"#%S", car.carNumberStr.c_str() );
                r = { xl, y-lineHeight/2, xl+m_numWidth, y+lineHeight/2 };
                m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
                m_brush->SetColor( car.isBuddy ? buddyCol : col );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
                xl += m_numWidth * 1.5f;

                // Irating
                swprintf( s, sizeof(s), L"%.1fk", (float)car.irating/1000.0f );
                r = { xr-m_iratingWidth, y-lineHeight/2, xr, y+lineHeight/2 };
                rr.rect = { r.left+1, r.top+1, r.right-1, r.bottom-1 };
                rr.radiusX = 3;
                rr.radiusY = 3;
                m_brush->SetColor( iratingBgCol );
                m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                m_textFormatSmall->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
                m_brush->SetColor( iratingTextCol );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormatSmall.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
                xr -= m_iratingWidth * 1.2f;

                // License/SR
                swprintf( s, sizeof(s), L"%C %.1f", car.licenseChar, car.licenseSR );
                r = { xr-m_safetyWidth, y-lineHeight/2, xr, y+lineHeight/2 };
                rr.rect = { r.left+1, r.top+1, r.right-1, r.bottom-1 };
                rr.radiusX = 3;
                rr.radiusY = 3;
                float4 c = car.licenseCol;
                c.a = licenseBgAlpha;
                m_brush->SetColor( c );
                m_renderTarget->FillRoundedRectangle( &rr, m_brush.Get() );
                m_textFormatSmall->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
                m_brush->SetColor( licenseTextCol );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormatSmall.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
                xr -= m_safetyWidth * 1.3f;

                // Delta
                swprintf( s, sizeof(s), L"%.1f", ci.delta );
                r = { xr-m_deltaWidth, y-lineHeight/2, xr, y+lineHeight/2 };
                m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
                m_brush->SetColor( col );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
                xr -= m_deltaWidth * 1.2f;

                // Name
                swprintf( s, sizeof(s), L"%S", car.userName.c_str() );
                r = { xl, y-lineHeight/2, xr, y+lineHeight/2 };
                m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_LEADING );
                m_brush->SetColor( col );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
            }

            m_renderTarget->EndDraw();
        }

    protected:

        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatSmall;

        float       m_posWidth = 0;
        float       m_numWidth = 0;
        float       m_deltaWidth = 0;
        float       m_iratingWidth = 0;
        float       m_safetyWidth = 0;
        
};
