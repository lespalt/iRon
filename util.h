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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <d2d1_3.h>
#include <dwrite.h>

#define HRCHECK( x_ ) do{ \
    HRESULT hr_ = x_; \
    if( FAILED(hr_) ) { \
        printf("ERROR: failed call to %s (%s:%d), hr=0x%x\n", #x_, __FILE__, __LINE__,hr_); \
        exit(1); \
    } } while(0)

struct float2
{
    union { float r; float x; };
    union { float g; float y; };
    float2() = default;
    float2( float _x, float _y ) : x(_x), y(_y) {}
    float2( const D2D1_POINT_2F& p ) : x(p.x), y(p.y) {}
    operator D2D1_POINT_2F() const { return {x,y}; }
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

struct float4
{
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    union { float a; float w; };
    float4() = default;
    float4( float _x, float _y, float _z, float _w ) : x(_x), y(_y), z(_z), w(_w) {}
    float4( const D2D1_COLOR_F& c ) : r(c.r), g(c.g), b(c.b), a(c.a) {}
    operator D2D1_COLOR_F() const { return {r,g,b,a}; }
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

inline bool loadFile( const std::string& fname, std::string& output )
{
    FILE* fp = fopen( fname.c_str(), "rb" );
    if( !fp )
        return false;

    fseek( fp, 0, SEEK_END );
    const long sz = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    char* buf = new char[sz];

    fread( buf, 1, sz, fp );
    fclose( fp );
    output = std::string( buf, sz );

    delete[] buf;
    return true;
}

inline bool saveFile( const std::string& fname, const std::string& s )
{
    FILE* fp = fopen( fname.c_str(), "wb" );
    if( !fp )
        return false;

    fwrite( s.data(), 1, s.length(), fp );

    fclose( fp );
    return true;
}

inline std::wstring toWide( const std::string& narrow )
{
    return std::wstring(narrow.begin(),narrow.end());
}

inline float2 computeTextExtent( const wchar_t* str, IDWriteFactory* factory, IDWriteTextFormat* textFormat )
{
    IDWriteTextLayout* textLayout = 0;

    factory->CreateTextLayout( str, (int)wcslen(str), textFormat, 99999, 99999, &textLayout );
    DWRITE_TEXT_METRICS m = {};
    textLayout->GetMetrics( &m );

    textLayout->Release();

    return float2( m.width, m.height );
}
