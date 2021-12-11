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

#include <stdio.h>
#include <string>
#include <gl/GL.h>

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif

struct float2
{
    union { float r; float x; };
    union { float g; float y; };
    float2() = default;
    float2( float _x, float _y ) : x(_x), y(_y) {}
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
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

inline void glerr()
{
    GLenum err = glGetError();
    if( err ) {
        printf("OpenGL error: %d\n", (int)err );
    } else {
        printf("OpenGL: no error\n");
    }
}

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
