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
            const float fontSize = g_cfg.getFloat( m_name, "font_size" );
            HRCHECK(m_dwriteFactory->CreateTextFormat( L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetLineSpacing( DWRITE_LINE_SPACING_METHOD_UNIFORM, fontSize, fontSize );
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
            const float driverEstTime = ir_CarIdxEstTime.getFloat(ir_session.driverCarIdx);
            for( int i=0; i<IR_MAX_CARS; ++i )
            {
                if( (i == ir_session.driverCarIdx || ir_CarIdxLap.getInt(i) >= 0) && !ir_session.cars[i].isPaceCar )
                {
                    CarInfo ci;
                    ci.carIdx = i;
                    ci.delta = driverEstTime - ir_CarIdxEstTime.getFloat(i);
                    relatives.push_back( ci );
                }
            }

            // Sort by delta
            std::sort( relatives.begin(), relatives.end(), 
                []( const CarInfo& a, const CarInfo&b ) { return a.delta < b.delta; } );

            // Locate our driver's index in the new array
            int driverCarInfoIdx = -1;
            for( int i=0; i<(int)relatives.size(); ++i )
            {
                if( relatives[i].carIdx == ir_session.driverCarIdx ) {
                    driverCarInfoIdx = i;
                    break;
                }
            }

            // Something's wrong if we didn't find our driver. Bail.
            if( driverCarInfoIdx < 0 )
                return;

            const float fontSize = g_cfg.getFloat( m_name, "font_size" );
            const float4 ownCol       = float4( 241.f/255.f, 175.f/255.f, 34.f/255.f, 1 );
            const float4 sameLapCol   = float4( 1, 1, 1, 1 );
            const float4 lapAheadCol  = float4( 231.f/255.f, 44.f/255.f, 44.f/255.f, 1 );


            // Display such that our driver is in the vertical center of the area where we're listing cars
            const float listingAreaTop  = m_height - 40.0f;
            const float listingAreaBot  = 40.0f;
            const float ydriver         = listingAreaBot + (listingAreaTop-listingAreaBot) / 2.0f - fontSize / 2.0f;
            const int   entriesAbove    = int( (listingAreaTop - ydriver) / fontSize ) - 1;

            float y = ydriver + entriesAbove * fontSize;

            m_renderTarget->BeginDraw();
            for( int i=driverCarInfoIdx-entriesAbove; i<(int)relatives.size() && y>=listingAreaBot; ++i, y-=fontSize )
            {
                if( i < 0 )
                    continue;

                const CarInfo& ci  = relatives[i];
                const Car&     car = ir_session.cars[ci.carIdx];

                float4 col = sameLapCol;
                if( i==driverCarInfoIdx )
                    col = ownCol;
                else if( ir_CarIdxOnPitRoad.getBool(ci.carIdx) )
                    col.a *= 0.5f;

                wchar_t s[1024];
                swprintf( s, sizeof(s), L"%2d #%2d %S %S   %.2f", ir_CarIdxPosition.getInt(ci.carIdx), car.carNumber, car.userName.c_str(), car.licenseStr.c_str(), ci.delta );

                D2D1_RECT_F r = { 0, y, (float)m_width, y+fontSize };
                m_brush->SetColor( col );
                m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get() );
            }
            m_renderTarget->EndDraw();
        }

    protected:

        Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
};
