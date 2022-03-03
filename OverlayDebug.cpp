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

#include "OverlayDebug.h"
#include <string>
#include <vector>
#include <stdarg.h>

struct DbgLine
{
    std::string s;
    float4      col;
};

static std::vector<DbgLine> g_dbgLines;

void dbg( const float4& color, const char* fmt, ... )
{
#ifndef _DEBUG
    return;
#endif

    va_list args;
    va_start( args, fmt );

    char s[2048];
    vsnprintf( s, sizeof(s), fmt, args );

    DbgLine line;
    line.s = s;
    line.col = color;
    g_dbgLines.emplace_back( line );

    va_end( args );
}

void dbg( const char* fmt, ... )
{
#ifndef _DEBUG
    return;
#endif

    va_list args;
    va_start( args, fmt );

    char s[2048];
    vsnprintf( s, sizeof(s), fmt, args );

    DbgLine line;
    line.s = s;
    line.col = float4(1,1,1,0.9f);
    g_dbgLines.emplace_back( line );

    va_end( args );
}


OverlayDebug::OverlayDebug()
    : Overlay("OverlayDebug")
{}

void OverlayDebug::onEnable()
{
    onConfigChanged();  // trigger font load
}

void OverlayDebug::onConfigChanged()
{
    HRCHECK(m_dwriteFactory->CreateTextFormat( L"Consolas", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"en-us", &m_textFormat ));
    m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
    m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );
}

void OverlayDebug::onUpdate()
{
    const float lineHeight = 20;

    m_renderTarget->BeginDraw();

    for( int i=0; i<(int)g_dbgLines.size(); ++i )
    {
        const DbgLine& line = g_dbgLines[i];

        const float y = 10 + lineHeight/2 + i*lineHeight;
        
        m_brush->SetColor( line.col );
        D2D1_RECT_F r = { 10, y-lineHeight/2, (float)m_width-10, y+lineHeight/2 };
        auto wstr = toWide( line.s );
        m_renderTarget->DrawTextA( wstr.c_str(), (int)wstr.size(), m_textFormat.Get(), &r, m_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP );
    }

    m_renderTarget->EndDraw();

    g_dbgLines.clear();
}

bool OverlayDebug::canEnableWhileNotDriving() const
{
    return true;
}

bool OverlayDebug::canEnableWhileDisconnected() const
{
    return true;
}
