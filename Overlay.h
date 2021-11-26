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

#include <windows.h>
#include <dwmapi.h>
#include <GL\gl.h>


static LRESULT CALLBACK windowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    return DefWindowProc(hwnd,msg,wparam,lparam);
}

class Overlay
{
    public:

        Overlay( const std::string name )
            : m_name( name )
        {
            const char* const wndclassName = "overlay";
            WNDCLASSEX wndclass = {};
            if( !GetClassInfoEx( 0, wndclassName, &wndclass ) )
            {
                wndclass.cbSize = sizeof(WNDCLASSEX);
                wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
                wndclass.lpfnWndProc = windowProc;
                wndclass.lpszClassName = wndclassName;
                wndclass.hbrBackground = CreateSolidBrush(0);
                RegisterClassEx(&wndclass);
            }

            m_hwnd = CreateWindowEx( WS_EX_TOPMOST, wndclassName, "", WS_POPUP|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, NULL, NULL );

            PIXELFORMATDESCRIPTOR pfd = {};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 24;
            pfd.cAlphaBits = 8;
            pfd.cDepthBits = 32;

            m_hdc = GetDC( m_hwnd );
            if( !SetPixelFormat(m_hdc, ChoosePixelFormat(m_hdc, &pfd), &pfd) )
                printf("SetPixelFormat failed\n");

            m_hglrc = wglCreateContext( m_hdc );
            if( !m_hglrc )
                printf("wglCreateContext failed\n");

            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

            // The following enables composition of our window into the desktop
            DWM_BLURBEHIND bb = {};
            bb.dwFlags = DWM_BB_ENABLE;
            bb.fEnable = true;
            DwmEnableBlurBehindWindow(m_hwnd, &bb);
        }

        virtual ~Overlay()
        {
            wglDeleteContext( m_hglrc );
            DestroyWindow( m_hwnd );
        }

        virtual void update() {}
        virtual void notifyConfigChanged() {}

    protected:

        std::string   m_name;
        HWND          m_hwnd = 0;
        HDC           m_hdc = 0;
        HGLRC         m_hglrc = 0;
};
