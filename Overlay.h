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

#include <windows.h>
#include <string>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dcomp.h>
#include <dwrite.h>
#include <wrl.h>
#include "util.h"

class Overlay
{
    public:

                        Overlay( const std::string name );
        virtual         ~Overlay();

        std::string     getName() const;
        virtual bool    canEnableWhileNotDriving() const;
        virtual bool    canEnableWhileDisconnected() const;

        void            enable( bool on );
        bool            isEnabled() const;

        void            enableUiEdit( bool on );
        bool            isUiEditEnabled() const;

        void            configChanged();
        void            sessionChanged();

        void            update();

        void            setWindowPosAndSize( int x, int y, int w, int h, bool callSetWindowPos=true );
        void            saveWindowPosAndSize();

    protected:

        virtual void    onEnable();
        virtual void    onDisable();
        virtual void    onUpdate();
        virtual void    onConfigChanged();
        virtual void    onSessionChanged();
        virtual float2  getDefaultSize();
        virtual bool    hasCustomBackground();

        std::string     m_name;
        HWND            m_hwnd = 0;
        bool            m_enabled = false;
        bool            m_uiEditEnabled = false;
        int             m_xpos = 0;
        int             m_ypos = 0;
        int             m_width = 0;
        int             m_height = 0;

        Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
        Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
        Microsoft::WRL::ComPtr<ID2D1Factory2>           m_d2dFactory;
        Microsoft::WRL::ComPtr<ID2D1RenderTarget>       m_renderTarget;
        Microsoft::WRL::ComPtr<IDCompositionDevice>     m_compositionDevice;
        Microsoft::WRL::ComPtr<IDCompositionTarget>     m_compositionTarget;
        Microsoft::WRL::ComPtr<IDCompositionVisual>     m_compositionVisual;
        Microsoft::WRL::ComPtr<IDWriteFactory>          m_dwriteFactory;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_brush;
};
