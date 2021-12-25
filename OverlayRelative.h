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
        {
            m_textFormat.Reset();
        }

        virtual void onConfigChanged()
        {
            const std::string font = g_cfg.getString( m_name, "font" );
            const float fontSize = g_cfg.getFloat( m_name, "font_size" );
            const int fontWeight = g_cfg.getInt( m_name, "font_weight" );
            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );
            //m_textFormat->SetLineSpacing( DWRITE_LINE_SPACING_METHOD_UNIFORM, 0, 0 );
        }

        virtual void onUpdate()
        {
            struct CarInfo {
                int carIdx;
                float delta;
            };
            std::vector<CarInfo> relatives;
            relatives.reserve( IR_MAX_CARS );

            // Populate cars with the ones for which a relative/delta comparison is valid
            const float selfEstTime     = ir_CarIdxEstTime.getFloat(ir_session.driverCarIdx);
            const float selfLapDistPct  = ir_CarIdxLapDistPct.getFloat(ir_session.driverCarIdx);

            for( int i=0; i<IR_MAX_CARS; ++i )
            {
                if( (i == ir_session.driverCarIdx || ir_CarIdxLap.getInt(i) >= 0) && !ir_session.cars[i].isPaceCar )
                {
                    // If the other car is up to half a lap in front, we consider the delta 'ahead', otherwise 'behind'.

                    // Does the delta between us and the other car span across the start/finish line?
                    const bool wrap = fabsf(ir_CarIdxLapDistPct.getFloat(i) - selfLapDistPct) > 0.5f;

                    float delta = 0;
                    const float L = ir_session.cars[i].carClassEstLapTime;
                    const float C = ir_CarIdxEstTime.getFloat(i);
                    const float S = selfEstTime;

                    if( wrap )
                        delta = S > C ? (C-S)+L : (C-S)-L;
                    else
                        delta = C - S;

                    CarInfo ci;
                    ci.carIdx = i;
                    ci.delta = delta;
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

            const float fontSize      = g_cfg.getFloat( m_name, "font_size" );
            const float lineSpacing   = g_cfg.getFloat( m_name, "line_spacing" );
            const float lineHeight    = fontSize + lineSpacing;
            const float4 selfCol       = float4( 241.f/255.f, 175.f/255.f, 34.f/255.f, 1 );
            const float4 sameLapCol   = float4( 1, 1, 1, 1 );
            const float4 lapAheadCol  = float4( 231.f/255.f, 44.f/255.f, 44.f/255.f, 1 );

            // Display such that our driver is in the vertical center of the area where we're listing cars
            const float listingAreaTop  = 40.0f;
            const float listingAreaBot  = m_height - 40.0f;
            const float yself         = listingAreaTop + (listingAreaBot-listingAreaTop) / 2.0f;
            const int   entriesAbove    = int( (yself - lineHeight/2 - listingAreaTop) / lineHeight );

            float y = yself - entriesAbove * lineHeight;

            m_renderTarget->BeginDraw();
            for( int i=selfCarInfoIdx-entriesAbove; i<(int)relatives.size() && y<=listingAreaBot-lineHeight/2; ++i, y+=lineHeight )
            {
                if( i < 0 )
                    continue;

                const CarInfo& ci  = relatives[i];
                const Car&     car = ir_session.cars[ci.carIdx];

                float4 col = sameLapCol;
                if( i==selfCarInfoIdx )
                    col = selfCol;
                else if( ir_CarIdxOnPitRoad.getBool(ci.carIdx) )
                    col.a *= 0.5f;

                wchar_t s[1024];
                swprintf( s, sizeof(s), L"%2d #%2d %S %S   %.2f", ir_CarIdxPosition.getInt(ci.carIdx), car.carNumber, car.userName.c_str(), car.licenseStr.c_str(), ci.delta );

                D2D1_RECT_F r = { 0, y-lineHeight/2, (float)m_width, y+lineHeight/2 };
                m_brush->SetColor( col );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get() );

                col.a *= 0.1f;
                m_brush->SetColor( col );
                m_renderTarget->DrawRectangle( &r, m_brush.Get() );
            }

            m_renderTarget->EndDraw();
        }

    protected:

        Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
};
