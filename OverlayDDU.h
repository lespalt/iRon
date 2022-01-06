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
#include "iracing.h"
#include "Config.h"

class OverlayDDU : public Overlay
{
    public:

        const float DefaultFontSize = 20;

        OverlayDDU()
            : Overlay("OverlayDDU")
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

            const std::string font = g_cfg.getString( m_name, "font", "Calibri Bold" );
            const float fontSize = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const int fontWeight = g_cfg.getInt( m_name, "font_weight", 500 );
            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, (DWRITE_FONT_WEIGHT)fontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );
        }

        virtual void onUpdate()
        {
            const int carIdx = ir_session.driverCarIdx;

            const float  fontSize           = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const float4 textCol            = g_cfg.getFloat4( m_name, "text_col", float4(1,1,1,0.9f) );

            wchar_t s[512];
            float y = 0;

            m_renderTarget->BeginDraw();

            m_brush->SetColor( textCol );

            char lapsStr[32];

            int currentLap = ir_isPreStart() ? 0 : ir_CarIdxLap.getInt(carIdx);
            int totalLaps = ir_SessionLapsTotal.getInt();
            int remainingLaps = ir_SessionLapsRemainEx.getInt();
            
            if( totalLaps == SHRT_MAX )
                sprintf( lapsStr, "--" );
            else
                sprintf( lapsStr, "%d", totalLaps );
            y = fontSize * 1.5f;
            swprintf( s, _countof(s), L"%d / %S", currentLap, lapsStr );
            m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), 10, (float)m_width-10, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

            if( remainingLaps == SHRT_MAX )
                sprintf( lapsStr, "--" );
            else
                sprintf( lapsStr, "%d", remainingLaps );
            y += fontSize * 1.5f;
            swprintf( s, _countof(s), L"Rem: %S", lapsStr );
            m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), 10, (float)m_width-10, y, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

            m_renderTarget->EndDraw();
        }

    protected:

        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;

        TextCache    m_text;
};

