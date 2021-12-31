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

#include "Overlay.h"
#include "Config.h"

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
            int carIdx = 0;
            int pos = 0;
            int lapDelta = 0;
        };
        std::vector<CarInfo> carInfo;
        carInfo.reserve( IR_MAX_CARS );

        const float  fontSize           = g_cfg.getFloat( m_name, "font_size" );
        const float  lineSpacing        = g_cfg.getFloat( m_name, "line_spacing" );
        const float  lineHeight         = fontSize + lineSpacing;

        // Compute positions
        for( int i=0; i<IR_MAX_CARS; ++i )
        {
            const Car& car = ir_session.cars[i];

            if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                continue;

            const int pos = ir_CarIdxPosition.getInt(i) > 0 ? ir_CarIdxPosition.getInt(i) : car.qualifyingResultPosition;

            CarInfo ci;
            ci.carIdx = i;
            ci.pos = pos;
            carInfo.push_back( ci );
        }

        std::sort( carInfo.begin(), carInfo.end(),
            []( const CarInfo& a, const CarInfo& b ) { return a.pos < b.pos; } );

        /*
        // Compute lap delta to leader
        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const int carIdxLeader = carInfo[0].carIdx;
            const int carIdx       = carInfo[i].carIdx;

            const int lapcountLeader = ir_CarIdxLap.getInt(carIdxLeader);
            const int lapcount       = ir_CarIdxLap.getInt(carIdx);

            int lapDelta = lapcount - lapcountLeader;

            // Correct for leader having passed start/finish while current car hasn't yet
            if( ir_CarIdxLapDistPct.getFloat(carIdxLeader) < ir_CarIdxLapDistPct.getFloat(carIdx) )
                lapDelta += 1;

            carInfo[i].lapDelta = lapDelta;
        }
        */

        // Compute lap delta to leader
        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const int carIdxLeader = carInfo[0].carIdx;
            const int carIdx       = carInfo[i].carIdx;

            const int lapcountLeader = ir_CarIdxLapCompleted.getInt(carIdxLeader);
            const int lapcount       = ir_CarIdxLapCompleted.getInt(carIdx);

            int lapDelta = lapcount - lapcountLeader;

            carInfo[i].lapDelta = lapDelta;
        }

        m_renderTarget->BeginDraw();

        for( int i=0; i<(int)carInfo.size(); ++i )
        {
            const CarInfo&  ci  = carInfo[i];
            const Car&      car = ir_session.cars[ci.carIdx];

            if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                continue;

            const int pos = ir_CarIdxPosition.getInt(ci.carIdx) > 0 ? ir_CarIdxPosition.getInt(ci.carIdx) : car.qualifyingResultPosition;

            D2D1_RECT_F r = {};

            float y = 10 + lineHeight/2 + i*lineHeight;

            wchar_t s[512];
            wchar_t diffToLeader[64];
            swprintf( diffToLeader, sizeof(diffToLeader), ci.lapDelta ? L"%.0fL" : L"%.3fs", ci.lapDelta ? (float)ci.lapDelta : ir_CarIdxF2Time.getFloat(ci.carIdx) );
            swprintf( s, sizeof(s), L"P%d  #%S  %S  %S  %.1fk %dx %s", pos, car.carNumberStr.c_str(), car.userName.c_str(), car.licenseStr.c_str(), (float)car.irating/1000.0f, car.incidentCount, diffToLeader );
            r = { 10, y-lineHeight/2, (float)m_width-10, y+lineHeight/2 };
            //m_textFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_TRAILING );
            m_brush->SetColor( float4(1,1,1,1) );
            m_renderTarget->DrawTextA( s, (int)wcslen(s), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
        }

        m_renderTarget->EndDraw();
    }

    virtual bool shouldEnableOnlyWhileDriving() const
    {
        return false;
    }

protected:

    Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;

};
