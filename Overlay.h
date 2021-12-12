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
#include <string>
#include "Font.h"

class Overlay
{
    public:

                        Overlay( const std::string name );
        virtual         ~Overlay();

        std::string     getName() const;
        bool            getUiEditEnabled() const;

        void            enable( bool on );
        void            enableUiEdit( bool on );
        void            configChanged();

        void            update();

        void            setWindowPosAndSize( int x, int y, int w, int h, bool callSetWindowPos=true );
        void            saveWindowPosAndSize();

    protected:

        void            resizeGlViewport();

        virtual void    onEnable();
        virtual void    onDisable();
        virtual void    onUpdate();
        virtual void    onConfigChanged();

        std::string     m_name;
        HWND            m_hwnd = 0;
        HDC             m_hdc = 0;
        HGLRC           m_hglrc = 0;
        bool            m_enabled = false;
        bool            m_uiEditEnabled = false;
        int             m_xpos = 0;
        int             m_ypos = 0;
        int             m_width = 0;
        int             m_height = 0;
};
