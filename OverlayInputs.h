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

class OverlayInputs : public Overlay
{
    public:

        static const int N = 250;

        OverlayInputs()
            : Overlay("OverlayInputs")
        {}

        void notifyConfigChanged()
        {
            const int x = g_cfg.getInt(m_name,"x_pos");
            const int y = g_cfg.getInt(m_name,"y_pos");
            const int w = g_cfg.getInt(m_name,"width");
            const int h = g_cfg.getInt(m_name,"height");
            SetWindowPos( m_hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE|SWP_SHOWWINDOW );

            m_throttleVtx.resize( w );
            m_brakeVtx.resize( w );
            for( int i=0; i<w; ++i )
            {
                m_throttleVtx[i].x = float(i);
                m_brakeVtx[i].x = float(i);
            }

            wglMakeCurrent( m_hdc, m_hglrc );
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0,w,0,h,1,-1);
        }

        void update()
        {
            wglMakeCurrent( m_hdc, m_hglrc );

            const float w = (float)g_cfg.getInt(m_name,"width");
            const float h = (float)g_cfg.getInt(m_name,"height");

            // Advance input vertices
            for( int i=0; i<(int)m_throttleVtx.size()-1; ++i )
                m_throttleVtx[i].y = m_throttleVtx[i+1].y;
            m_throttleVtx[(int)m_throttleVtx.size()-1].y = ir_Throttle.getFloat();

            for( int i=0; i<(int)m_brakeVtx.size()-1; ++i )
                m_brakeVtx[i].y = m_brakeVtx[i+1].y;
            m_brakeVtx[(int)m_brakeVtx.size()-1].y = ir_Brake.getFloat();

            // Clear background
            float4 bgcol = g_cfg.getFloat4( m_name, "background_col" );
            glClearColor( bgcol.r, bgcol.g, bgcol.b, bgcol.a );
            glClear(GL_COLOR_BUFFER_BIT);

            // Throttle fill
            float4 col = g_cfg.getFloat4( m_name, "throttle_fill_col" );
            glColor4fv( &col );
            glBegin( GL_TRIANGLE_STRIP );
            for( int i=0; i<(int)m_throttleVtx.size(); ++i )
            {
                glVertex2f(m_throttleVtx[i].x,m_throttleVtx[i].y*h);
                glVertex2f(m_throttleVtx[i].x,0);
            }
            glEnd();

            // Brake fill
            col = g_cfg.getFloat4( m_name, "brake_fill_col" );
            glColor4fv( &col );
            glBegin( GL_TRIANGLE_STRIP );
            for( int i=0; i<(int)m_brakeVtx.size(); ++i )
            {
                glVertex2f(m_brakeVtx[i].x,m_brakeVtx[i].y*h);
                glVertex2f(m_throttleVtx[i].x,0);
            }
            glEnd();

            glLineWidth( 2 );

            // Throttle line
            col = g_cfg.getFloat4( m_name, "throttle_col" );
            glColor4fv( &col );
            glBegin( GL_LINE_STRIP );
            for( int i=0; i<(int)m_throttleVtx.size(); ++i )
                glVertex2f(m_throttleVtx[i].x,m_throttleVtx[i].y*h);
            glEnd();

            // Brake line
            col = g_cfg.getFloat4( m_name, "brake_col" );
            glColor4fv( &col );
            glBegin( GL_LINE_STRIP );
            for( int i=0; i<(int)m_brakeVtx.size(); ++i )
                glVertex2f(m_brakeVtx[i].x,m_brakeVtx[i].y*h);
            glEnd();

            glFlush();
            SwapBuffers( m_hdc );
        }

        std::vector<float2> m_throttleVtx;
        std::vector<float2> m_brakeVtx;
};
