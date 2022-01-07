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

        const float DefaultFontSize = 17;

        OverlayDDU()
            : Overlay("OverlayDDU")
        {}

        bool canEnableWhileDisconnected() const { return true; }
        bool canEnableWhileNotDriving() const { return true; }

    protected:

        struct Box
        {
            float x0 = 0;
            float x1 = 0;
            float y0 = 0;
            float y1 = 0;
            float w = 0;
            float h = 0;
            std::string title;
        };

        virtual float2 getDefaultSize()
        {
            return float2(809,166);
        }

        virtual void onEnable()
        {
            onConfigChanged();
        }

        virtual void onDisable()
        {
            m_text.reset();
        }

        virtual void onConfigChanged()
        {
            m_text.reset( m_dwriteFactory.Get() );

            // Font stuff
            const std::string font = g_cfg.getString( m_name, "font", "Arial" );
            const float fontSize = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const int fontWeight = g_cfg.getInt( m_name, "font_weight", 500 );
            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormat ));
            m_textFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormat->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", &m_textFormatBold ));
            m_textFormatBold->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatBold->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*1.2f, L"en-us", &m_textFormatLarge ));
            m_textFormatLarge->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatLarge->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*0.7f, L"en-us", &m_textFormatSmall ));
            m_textFormatSmall->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatSmall->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*0.6f, L"en-us", &m_textFormatVerySmall ));
            m_textFormatVerySmall->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatVerySmall->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            // Box geometries
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  geometrySink;
            m_d2dFactory->CreatePathGeometry( &m_pathGeometry );
            m_pathGeometry->Open( &geometrySink );

            m_boxGear = makeBox( 0.5f-0.045f, 0.5f+0.045f, 0.11f, 0.64f, "" );
            addBoxFigure( geometrySink.Get(), m_boxGear );

            m_boxLaps = makeBox( 0.5f-0.165f, 0.5f-0.05f, 0.11f, 0.64f, "Lap" );
            addBoxFigure( geometrySink.Get(), m_boxLaps );

            m_boxPos = makeBox( 0.5f-0.23f, 0.5f-0.17f, 0.4f, 0.64f, "Pos" );
            addBoxFigure( geometrySink.Get(), m_boxPos );

            m_boxBest = makeBox( 0.5f+0.05f, 0.5f+0.16f, 0.11f, 0.35f, "Best" );
            addBoxFigure( geometrySink.Get(), m_boxBest );

            m_boxLast = makeBox( 0.5f+0.05f, 0.5f+0.16f, 0.4f, 0.64f, "Last" );
            addBoxFigure( geometrySink.Get(), m_boxLast );

            m_boxP1Last = makeBox( 0.5f+0.05f, 0.5f+0.16f, 0.69f, 0.93f, "P1 Last" );
            addBoxFigure( geometrySink.Get(), m_boxP1Last );

            m_boxFuel = makeBox( 0.5f+0.165f, 0.5f+0.33f, 0.4f, 0.93f, "Fuel" );
            addBoxFigure( geometrySink.Get(), m_boxFuel );

            m_boxTires = makeBox( 0.5f+0.335f, 0.5f+0.425f, 0.69f, 0.93f, "Tires" );
            addBoxFigure( geometrySink.Get(), m_boxTires );

            m_boxDelta = makeBox( 0.5f-0.045f, 0.5f+0.045f, 0.69f, 0.93f, "vs Best" );
            addBoxFigure( geometrySink.Get(), m_boxDelta );

            m_boxSession = makeBox( 0.5f-0.165f, 0.5f-0.05f, 0.69f, 0.93f, "Session" );
            addBoxFigure( geometrySink.Get(), m_boxSession );

            m_boxInc = makeBox( 0.5f-0.23f, 0.5f-0.17f, 0.69f, 0.93f, "Inc" );
            addBoxFigure( geometrySink.Get(), m_boxInc );

            m_boxBias = makeBox( 0.5f-0.295f, 0.5f-0.235f, 0.69f, 0.93f, "Bias" );
            addBoxFigure( geometrySink.Get(), m_boxBias );

            geometrySink->Close();
        }

        virtual void onUpdate()
        {
            const int carIdx = ir_session.driverCarIdx;

            const DWORD tickCount = GetTickCount();

            const int currentLap = ir_isPreStart() ? 0 : std::max(0,ir_CarIdxLap.getInt(carIdx));
            const bool lapChanged = currentLap != m_prevCurrentLap;
            m_prevCurrentLap = currentLap;
            if( lapChanged )
                m_lastLapChangeTickCount = tickCount;

            const float  fontSize           = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const float4 textCol            = g_cfg.getFloat4( m_name, "text_col", float4(1,1,1,0.9f) );
            const int    deltaHoldMsec      = g_cfg.getInt( m_name, "delta_hold_msec", 3000 );

            wchar_t s[512];
            float y = 0;

            m_renderTarget->BeginDraw();
            m_brush->SetColor( textCol );            
            
            // Laps
            {
                char lapsStr[32];
                
                const int totalLaps = ir_SessionLapsTotal.getInt();
                const int remainingLaps = ir_SessionLapsRemainEx.getInt();
            
                if( totalLaps == SHRT_MAX )
                    sprintf( lapsStr, "--" );
                else
                    sprintf( lapsStr, "%d", totalLaps );
                y = fontSize * 1.5f;
                swprintf( s, _countof(s), L"%d / %S", currentLap, lapsStr );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxLaps.x0, m_boxLaps.x1, m_boxLaps.y0+m_boxLaps.h*0.25f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

                if( remainingLaps == SHRT_MAX )
                    sprintf( lapsStr, "--" );
                else
                    sprintf( lapsStr, "%d", remainingLaps );
                y += fontSize * 1.5f;
                swprintf( s, _countof(s), L"%S", lapsStr );
                m_text.render( m_renderTarget.Get(), s, m_textFormatLarge.Get(), m_boxLaps.x0, m_boxLaps.x1, m_boxLaps.y0+m_boxLaps.h*0.55f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

                m_text.render( m_renderTarget.Get(), L"REMAIN", m_textFormatVerySmall.Get(), m_boxLaps.x0, m_boxLaps.x1, m_boxLaps.y0+m_boxLaps.h*0.75f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Position
            {
                const int pos = ir_getPosition( ir_session.driverCarIdx );
                if( pos )
                {
                    swprintf( s, _countof(s), L"%d", pos );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatLarge.Get(), m_boxPos.x0, m_boxPos.x1, m_boxPos.y0+m_boxPos.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }
            }

            // Best time
            {
                
            }

            // Last time
            {
                
            }

            // P1's Last time
            {
                
            }

            // Fuel
            {
                
            }

            // Tires
            {
                
            }

            // Delta
            {
                s[0] = L'\0';
                float t = 0;

                if( m_prevDeltaOk && int(tickCount-m_lastLapChangeTickCount) <= deltaHoldMsec )
                {
                    t = m_prevDelta;
                    swprintf( s, _countof(s), L"%+5.3f", t );
                }
                else if( ir_LapDeltaToBestLap_OK.getBool() )
                {
                    t = ir_LapDeltaToBestLap.getFloat();
                    swprintf( s, _countof(s), L"%+4.2f", t );
                    m_prevDelta = t;
                    m_prevDeltaOk = true;
                }
                else
                {
                    m_prevDeltaOk = false;
                }

                if( m_prevDeltaOk )
                {
                    D2D1_RECT_F r = { m_boxDelta.x0, m_boxDelta.y0, m_boxDelta.x1, m_boxDelta.y1 };
                    m_brush->SetColor( t <= 0 ? float4(0,0.8f,0,0.8f) : float4(0.8f,0,0,0.8f) );
                    m_renderTarget->FillRectangle( &r, m_brush.Get() );
                    m_brush->SetColor( textCol );
                }

                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxDelta.x0, m_boxDelta.x1, m_boxDelta.y0+m_boxDelta.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Session
            {                   
                // iRacing will sometimes set a really high time limit in addition to a lap limit,
                // forcing us to choose whether we assume a session is naturally time-limited or lap-limited.
                // So once again use a heuristic to guess what makes the most sense to display.

                bool countdown = false;
                if( ir_session.isUnlimitedLaps && !ir_session.isUnlimitedTime )
                    countdown = true;
                if( !ir_session.isUnlimitedLaps && !ir_session.isUnlimitedTime && ir_session.sessionType!=SessionType::RACE )
                    countdown = true;

                const double sessionTime = countdown ? ir_SessionTimeRemain.getDouble() : ir_SessionTime.getDouble();

                const int    hours = int( sessionTime / 3600.0 );
                const int    mins  = int( sessionTime / 60.0 ) % 60;
                const int    secs  = (int)fmod( sessionTime, 60.0 );
                if( hours )
                    swprintf( s, _countof(s), L"%d:%02d:%d", hours, mins, secs );
                else
                    swprintf( s, _countof(s), L"%02d:%02d", mins, secs );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxSession.x0, m_boxSession.x1, m_boxSession.y0+m_boxSession.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Incidents
            {
                const int inc = ir_PlayerCarMyIncidentCount.getInt();
                swprintf( s, _countof(s), L"%dx", inc );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxInc.x0, m_boxInc.x1, m_boxInc.y0+m_boxInc.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Brake bias
            {
                const float bias = ir_dcBrakeBias.getFloat();
                swprintf( s, _countof(s), L"%+2.1f", bias );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxBias.x0, m_boxBias.x1, m_boxBias.y0+m_boxBias.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Draw all the box outlines and titles
            m_renderTarget->DrawGeometry( m_pathGeometry.Get(), m_brush.Get() );
            m_text.render( m_renderTarget.Get(), L"Lap",     m_textFormatSmall.Get(), m_boxLaps.x0, m_boxLaps.x1, m_boxLaps.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Pos",     m_textFormatSmall.Get(), m_boxPos.x0, m_boxPos.x1, m_boxPos.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Best",    m_textFormatSmall.Get(), m_boxBest.x0, m_boxBest.x1, m_boxBest.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Last",    m_textFormatSmall.Get(), m_boxLast.x0, m_boxLast.x1, m_boxLast.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"P1 Last", m_textFormatSmall.Get(), m_boxP1Last.x0, m_boxP1Last.x1, m_boxP1Last.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Fuel",    m_textFormatSmall.Get(), m_boxFuel.x0, m_boxFuel.x1, m_boxFuel.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Tires",   m_textFormatSmall.Get(), m_boxTires.x0, m_boxTires.x1, m_boxTires.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"vs Best", m_textFormatSmall.Get(), m_boxDelta.x0, m_boxDelta.x1, m_boxDelta.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Session", m_textFormatSmall.Get(), m_boxSession.x0, m_boxSession.x1, m_boxSession.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Bias",    m_textFormatSmall.Get(), m_boxBias.x0, m_boxBias.x1, m_boxBias.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            m_text.render( m_renderTarget.Get(), L"Inc",     m_textFormatSmall.Get(), m_boxInc.x0, m_boxInc.x1, m_boxInc.y0, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

            m_renderTarget->EndDraw();
        }

        void addBoxFigure( ID2D1GeometrySink* geometrySink, const Box& box )
        {
            if( !box.title.empty() )
            {
                const float hctr = (box.x0 + box.x1) * 0.5f;
                const float titleWidth = 6 + m_text.getExtent( toWide(box.title).c_str(), m_textFormat.Get(), box.x0, box.x1, DWRITE_TEXT_ALIGNMENT_CENTER ).x;
                geometrySink->BeginFigure( float2(hctr-titleWidth/2,box.y0), D2D1_FIGURE_BEGIN_HOLLOW );
                geometrySink->AddLine( float2(box.x0,box.y0) );
                geometrySink->AddLine( float2(box.x0,box.y1) );
                geometrySink->AddLine( float2(box.x1,box.y1) );
                geometrySink->AddLine( float2(box.x1,box.y0) );
                geometrySink->AddLine( float2(hctr+titleWidth/2,box.y0) );
                geometrySink->EndFigure( D2D1_FIGURE_END_OPEN );
            }
            else
            {
                geometrySink->BeginFigure( float2(box.x0,box.y0), D2D1_FIGURE_BEGIN_HOLLOW );
                geometrySink->AddLine( float2(box.x0,box.y1) );
                geometrySink->AddLine( float2(box.x1,box.y1) );
                geometrySink->AddLine( float2(box.x1,box.y0) );
                geometrySink->EndFigure( D2D1_FIGURE_END_CLOSED );
            }
        }

        float r2ax( float rx )
        {
            return rx * (float)m_width;
        }

        float r2ay( float ry )
        {
            return ry * (float)m_height;
        }

        Box makeBox( float x0, float x1, float y0, float y1, const std::string& title )
        {
            Box r;

            if( x0 > x1 || y0 > y1 )
                return r;

            r.x0 = r2ax( x0 );
            r.x1 = r2ax( x1 );
            r.y0 = r2ay( y0 );
            r.y1 = r2ay( y1 );
            r.w = r.x1 - r.x0;
            r.h = r.y1 - r.y0;
            r.title = title;
            return r;
        }

    protected:

        Box m_boxGear;
        Box m_boxLaps;
        Box m_boxPos;
        Box m_boxBest;
        Box m_boxLast;
        Box m_boxP1Last;
        Box m_boxDelta;
        Box m_boxSession;
        Box m_boxInc;
        Box m_boxBias;
        Box m_boxFuel;
        Box m_boxTires;

        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormat;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatBold;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatLarge;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatSmall;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatVerySmall;
        Microsoft::WRL::ComPtr<ID2D1PathGeometry1> m_pathGeometry;

        TextCache    m_text;

        int     m_prevCurrentLap = 0;
        DWORD   m_lastLapChangeTickCount = 0;

        float   m_prevDelta = 0;
        bool    m_prevDeltaOk = false;
};

