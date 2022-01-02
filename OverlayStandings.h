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

#include <assert.h>
#include "Overlay.h"
#include "Config.h"
#include "OverlayDebug.h"

class OverlayStandings : public Overlay
{
public:

    OverlayStandings()
        : Overlay("OverlayStandings")
    {}

    virtual void onEnable()
    {
        onConfigChanged();  // trigger font load
    }

    virtual void onConfigChanged()
    {
        const std::string font = g_cfg.getString( m_name, "font" );
        const float fontSize = g_cfg.getFloat( m_name, "font_size" );
        const int fontWeight = g_cfg.getInt( m_name, "font_weight" );
        HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
        m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
        m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );
    }

    virtual void onUpdate()
    {
        struct CarInfo {
            int     carIdx = 0;
            int     lapCount = 0;
            float   estTimeAroundLap = 0;
            int     lapDelta = 0;
            float   delta = 0;
        };
        std::vector<CarInfo> carInfo;
        carInfo.reserve( IR_MAX_CARS );

        // Init array
        for( int i=0; i<IR_MAX_CARS; ++i )
        {
            const Car& car = ir_session.cars[i];

            if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                continue;

            CarInfo ci;
            ci.carIdx = i;
            ci.lapCount = ir_isPacing() ? 0 : ir_CarIdxLap.getInt(i);   // correct for iRacing starting to count laps during pacing just to reset it again
            ci.estTimeAroundLap = ir_CarIdxEstTime.getFloat(i);
            carInfo.push_back( ci );
        }

        // Sort by position
        std::sort( carInfo.begin(), carInfo.end(),
            []( const CarInfo& a, const CarInfo& b ) { 
                if( a.lapCount != b.lapCount )
                    return a.lapCount > b.lapCount;
                return a.estTimeAroundLap > b.estTimeAroundLap;
            } );

        // Compute delta to leader
        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const CarInfo& ciLeader = carInfo[0];
            CarInfo&       ci       = carInfo[i];

            if( ci.lapCount == ciLeader.lapCount )
            {
                assert( ciLeader.estTimeAroundLap >= ci.estTimeAroundLap );
                ci.lapDelta = 0;
                ci.delta = ci.estTimeAroundLap - ciLeader.estTimeAroundLap;
            }
            else
            {
                assert( ciLeader.lapCount > ci.lapCount );
                if( ciLeader.estTimeAroundLap > ci.estTimeAroundLap ) {
                    ci.lapDelta = ci.lapCount - ciLeader.lapCount;
                    ci.delta = ci.estTimeAroundLap - ciLeader.estTimeAroundLap;
                } else {
                    ci.lapDelta = ci.lapCount + 1 - ciLeader.lapCount;
                    ci.delta =  ci.estTimeAroundLap - ciLeader.estTimeAroundLap - ir_estimateLaptime();
                }
            }
        }

        const float  fontSize           = g_cfg.getFloat( m_name, "font_size" );
        const float  lineSpacing        = g_cfg.getFloat( m_name, "line_spacing" );
        const float  lineHeight         = fontSize + lineSpacing;

        m_renderTarget->BeginDraw();

        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const CarInfo&  ci  = carInfo[i];
            const Car&      car = ir_session.cars[ci.carIdx];

            D2D1_RECT_F r = {};

            float y = 10 + lineHeight/2 + i*lineHeight;

            wchar_t s[512];
            wchar_t diffToLeader[64];
            swprintf( diffToLeader, sizeof(diffToLeader), ci.lapDelta ? L"%.0fL" : L"%.3fs", ci.lapDelta ? (float)ci.lapDelta : ci.delta );
            swprintf( s, sizeof(s), L"P%d  #%S  %S  %S  %.1fk %dx %s", i+1, car.carNumberStr.c_str(), car.userName.c_str(), car.licenseStr.c_str(), (float)car.irating/1000.0f, car.incidentCount, diffToLeader );
            r = { 10, y-lineHeight/2, (float)m_width-10, y+lineHeight/2 };
            //m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
            m_brush->SetColor( float4(1,1,1,1) );
            if( ci.carIdx == ir_session.driverCarIdx )
                m_brush->SetColor(float4(1,1,0,1));
            m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );

            float4 dbgcol = float4(1,1,1,0.9f);
            if( ci.carIdx == ir_session.driverCarIdx )
                dbgcol = float4(1,1,0,0.9f);
           // dbg( dbgcol, "%d %s", ir_session.cars[ci.carIdx].lapsComplete, car.userName.c_str() );
        }

        m_renderTarget->EndDraw();
    }

    virtual bool canEnableWhileNotDriving() const
    {
        return true;
    }

protected:

    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;

};
