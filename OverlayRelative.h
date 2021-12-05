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
#include "Font.h"

class OverlayRelative : public Overlay
{
    public:

        OverlayRelative()
            : Overlay("OverlayRelative")
        {}

        virtual void onEnable()
        {
            m_fnt.load("arial-black");
        }

        virtual void onDisable()
        {
            m_fnt.unload();
        }

        virtual void onConfigChanged()
        {
            // Position/dimensions might have changed
            const int x = g_cfg.getInt(m_name,"x_pos");
            const int y = g_cfg.getInt(m_name,"y_pos");
            const int w = g_cfg.getInt(m_name,"width");
            const int h = g_cfg.getInt(m_name,"height");
            setWindowPosAndSize( x, y, w, h );
        }

        virtual void update()
        {
            if( !m_enabled )
                return;

            wglMakeCurrent( m_hdc, m_hglrc );

            // Clear background
            float4 bgcol = g_cfg.getFloat4( m_name, "background_col" );
            glClearColor( bgcol.r, bgcol.g, bgcol.b, bgcol.a );
            glClear(GL_COLOR_BUFFER_BIT);

            
            m_fnt.render( "äLTo testgI.!?ü+Frènch", 0, 0 );

            glFlush();
            SwapBuffers( m_hdc );
        }

    protected:

        Font    m_fnt;
};
