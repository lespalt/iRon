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

        virtual bool    canEnableWhileNotDriving() const { return true; }
        virtual bool    canEnableWhileDisconnected() const { return true; }


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

            HRCHECK(m_dwriteFactory->CreateTextFormat( toWide(font).c_str(), NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize*3.0f, L"en-us", &m_textFormatGear ));
            m_textFormatGear->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
            m_textFormatGear->SetWordWrapping( DWRITE_WORD_WRAPPING_NO_WRAP );

            // Box geometries
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  geometrySink;
            m_d2dFactory->CreatePathGeometry( &m_pathGeometry );
            m_pathGeometry->Open( &geometrySink );

            const float hgap = 0.005f;
            const float vgap = 0.05f;
            const float vtop = 0.11f;
            const float gearw = 0.09f;
            const float w1 = 0.06f;
            const float w2 = 0.12f;
            const float h1 = 0.24f;
            const float h2 = 2*h1+vgap;
            
            m_boxGear = makeBox( 0.5f-gearw/2, gearw, vtop, 0.53f, "" );
            addBoxFigure( geometrySink.Get(), m_boxGear );

            m_boxDelta = makeBox( 0.5f-gearw/2, gearw, vtop+2*vgap+2*h1, h1, "vs Best" );
            addBoxFigure( geometrySink.Get(), m_boxDelta );
            
            m_boxBest = makeBox( 0.5f-gearw/2-hgap-w2, w2, vtop, h1, "Best" );
            addBoxFigure( geometrySink.Get(), m_boxBest );
            
            m_boxLast = makeBox( 0.5f-gearw/2-hgap-w2, w2, vtop+vgap+h1, h1, "Last" );
            addBoxFigure( geometrySink.Get(), m_boxLast );

            m_boxP1Last = makeBox( 0.5f-gearw/2-hgap-w2, w2, vtop+2*vgap+2*h1, h1, "P1 Last" );
            addBoxFigure( geometrySink.Get(), m_boxP1Last );

            m_boxLaps = makeBox( 0.5f-gearw/2-2*hgap-2*w2, w2, vtop+vgap+h1, h2, "Lap" );
            addBoxFigure( geometrySink.Get(), m_boxLaps );

            m_boxSession = makeBox( 0.5f-gearw/2-2*hgap-2*w2, w2, vtop+h1/3, h1*2.f/3.f, "Session" );
            addBoxFigure( geometrySink.Get(), m_boxSession );

            m_boxPos = makeBox( 0.5f-gearw/2-3*hgap-2*w2-w1, w1, vtop+vgap+h1, h1, "Pos" );
            addBoxFigure( geometrySink.Get(), m_boxPos );

            m_boxInc = makeBox( 0.5f-gearw/2-3*hgap-2*w2-w1, w1, vtop+2*vgap+2*h1, h1, "Inc" );
            addBoxFigure( geometrySink.Get(), m_boxInc );

            m_boxFuel = makeBox( 0.5f+gearw/2+hgap, w2*1.5f, vtop+h1/3, 2*vgap+h1*8.0f/3.0f, "Fuel" );
            addBoxFigure( geometrySink.Get(), m_boxFuel );

            m_boxBias = makeBox( 0.5f+gearw/2+2*hgap+w2*1.5f, w1, vtop+vgap+h1, h1, "Bias" );
            addBoxFigure( geometrySink.Get(), m_boxBias );
            
            m_boxTires = makeBox( 0.5f+gearw/2+2*hgap+w2*1.5f, w2, vtop+2*vgap+2*h1, h1, "Tires" );
            addBoxFigure( geometrySink.Get(), m_boxTires );

            geometrySink->Close();
        }

        virtual void onUpdate()
        {
            const int carIdx = ir_session.driverCarIdx;

            const DWORD tickCount = GetTickCount();

            const int currentLap = ir_isPreStart() ? 0 : std::max(0,ir_CarIdxLap.getInt(carIdx));
            const bool newLapCount = currentLap != m_prevCurrentLap;
            m_prevCurrentLap = currentLap;
            if( newLapCount )
                m_lastLapChangeTickCount = tickCount;

            const float  fontSize           = g_cfg.getFloat( m_name, "font_size", DefaultFontSize );
            const float4 outlineCol         = g_cfg.getFloat4( m_name, "outline_col", float4(0.7f,0.7f,0.7f,0.9f) );
            const float4 textCol            = g_cfg.getFloat4( m_name, "text_col", float4(1,1,1,0.9f) );
            const int    deltaHoldMsec      = g_cfg.getInt( m_name, "delta_hold_msec", 3000 );
            const float4 goodCol            = float4(0,0.8f,0,0.5f);
            const float4 badCol             = float4(0.8f,0,0,0.5f);
            const float4 fastestCol         = float4(0.8f,0,0.8f,0.5f);
            const float4 serviceCol         = float4(0.36f,0.61f,0.84f,1);
            const float4 warnCol            = float4(1,0.6f,0,1);

            wchar_t s[512];
            float y = 0;

            m_renderTarget->BeginDraw();
            m_brush->SetColor( textCol );      
            
            // Gear & Speed
            {
                const int gear = ir_Gear.getInt();
                char gearC = ' ';
                if( gear == -1 )
                    gearC = 'R';
                else if( gear == 0 )
                    gearC = 'N';
                else
                    gearC = char(gear + 48);
                swprintf( s, _countof(s), L"%C", gearC );
                m_text.render( m_renderTarget.Get(), s, m_textFormatGear.Get(), m_boxGear.x0, m_boxGear.x1, m_boxGear.y0+m_boxGear.h*0.41f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

                const float speedMps = ir_Speed.getFloat();
                if( speedMps >= 0 )
                {
                    float speed = 0;
                    if( ir_DisplayUnits.getInt() == 1 )
                        speed = speedMps * 3.6f;
                    else
                        speed = speedMps * 2.23694f;
                    swprintf( s, _countof(s), L"%d", (int)(speed+0.5f) );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatBold.Get(), m_boxGear.x0, m_boxGear.x1, m_boxGear.y0+m_boxGear.h*0.8f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }
            }
            
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
                // Figure out if we have the fastest lap across all cars
                bool haveFastestLap = false;
                {
                    int fastestLapCarIdx = -1;
                    float fastest = FLT_MAX;
                    for( int i=0; i<IR_MAX_CARS; ++i )
                    {
                        const Car& car = ir_session.cars[i];
                        if( car.isPaceCar || car.isSpectator || car.userName.empty() )
                            continue;

                        const float best = ir_CarIdxBestLapTime.getFloat(i);
                        if( best > 0 && best < fastest ) {
                            fastest = best;
                            fastestLapCarIdx = i;
                        }
                    }
                    haveFastestLap = fastestLapCarIdx == ir_session.driverCarIdx;
                }

                const float t = ir_LapBestLapTime.getFloat();
                if( t > 0 )
                {
                    D2D1_RECT_F r = { m_boxBest.x0, m_boxBest.y0, m_boxBest.x1, m_boxBest.y1 };
                    m_brush->SetColor( haveFastestLap ? fastestCol : goodCol );
                    m_renderTarget->FillRectangle( &r, m_brush.Get() );
                    m_brush->SetColor( textCol );

                    std::string str = formatLaptime( t );
                    m_text.render( m_renderTarget.Get(), toWide(str).c_str(), m_textFormat.Get(), m_boxBest.x0, m_boxBest.x1, m_boxBest.y0+m_boxBest.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }
            }

            // Last time
            {
                const float t = ir_LapLastLapTime.getFloat();
                if( t > 0 )
                {
                    std::string str = formatLaptime( t );
                    m_text.render( m_renderTarget.Get(), toWide(str).c_str(), m_textFormat.Get(), m_boxLast.x0, m_boxLast.x1, m_boxLast.y0+m_boxLast.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                }
            }

            // P1's Last time
            {
                // Figure out who's P1
                int p1carIdx = -1;
                for( int i=0; i<IR_MAX_CARS; ++i )
                {
                    if( ir_getPosition(i) == 1 ) {
                        p1carIdx = i;
                        break;
                    }
                }

                if( p1carIdx >= 0 )
                {
                    const float t = ir_CarIdxLastLapTime.getFloat( p1carIdx );
                    if( t > 0 )
                    {
                        std::string str = formatLaptime( t );
                        m_text.render( m_renderTarget.Get(), toWide(str).c_str(), m_textFormat.Get(), m_boxP1Last.x0, m_boxP1Last.x1, m_boxP1Last.y0+m_boxP1Last.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                    }
                }
            }

            // Fuel
            {
                m_text.render( m_renderTarget.Get(), L"Est Laps",   m_textFormatSmall.Get(), m_boxFuel.x0+7, m_boxFuel.x1, m_boxFuel.y0+m_boxFuel.h*2.1f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
                m_text.render( m_renderTarget.Get(), L"Remaining",  m_textFormatSmall.Get(), m_boxFuel.x0+7, m_boxFuel.x1, m_boxFuel.y0+m_boxFuel.h*4.2f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
                m_text.render( m_renderTarget.Get(), L"Per Lap",    m_textFormatSmall.Get(), m_boxFuel.x0+7, m_boxFuel.x1, m_boxFuel.y0+m_boxFuel.h*6.2f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
                m_text.render( m_renderTarget.Get(), L"+To Finish", m_textFormatSmall.Get(), m_boxFuel.x0+7, m_boxFuel.x1, m_boxFuel.y0+m_boxFuel.h*8.4f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );
                m_text.render( m_renderTarget.Get(), L"+Add",       m_textFormatSmall.Get(), m_boxFuel.x0+7, m_boxFuel.x1, m_boxFuel.y0+m_boxFuel.h*10.5f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_LEADING );

                const float safetyFact = g_cfg.getFloat( m_name, "fuel_estimate_safety_factor", 1.05f );
                const bool  useGallons = ir_DisplayUnits.getInt() == 0;
                const float remainingFuel = ir_FuelLevel.getFloat();

                if( newLapCount )
                {
                    const float usedLastLap = std::max( 0.0f, m_lapStartRemainingFuel - remainingFuel );
                    m_lapStartRemainingFuel = remainingFuel;
                    m_maxPerLapFuelUsed = std::max( m_maxPerLapFuelUsed, usedLastLap );
                }

                // Est Laps
                const float perLapConsEst = m_maxPerLapFuelUsed * safetyFact;  // conservative estimate of per-lap use for further calculations
                if( perLapConsEst > 0 )
                {
                    const float estLaps = remainingFuel / perLapConsEst;
                    swprintf( s, _countof(s), L"%.1f", estLaps );
                    m_text.render( m_renderTarget.Get(), s, m_textFormatBold.Get(), m_boxFuel.x0, m_boxFuel.x1-5, m_boxFuel.y0+m_boxFuel.h*2.1f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                }

                // Remaining
                if( remainingFuel >= 0 )
                {
                    swprintf( s, _countof(s), useGallons ? L"%.1f gl" : L"%.1f lt", remainingFuel );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxFuel.x0, m_boxFuel.x1-5, m_boxFuel.y0+m_boxFuel.h*4.2f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                }

                // Per Lap
                if( m_maxPerLapFuelUsed > 0 )
                {
                    float val = m_maxPerLapFuelUsed;
                    if( useGallons )
                        val *= 0.264172f;
                    swprintf( s, _countof(s), useGallons ? L"%.1f gl" : L"%.1f lt", val );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxFuel.x0, m_boxFuel.x1-5, m_boxFuel.y0+m_boxFuel.h*6.3f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                }

                // To Finish
                const int remainingLaps = ir_SessionLapsRemainEx.getInt();  // TODO: replace with estimate when session is time-limited
                if( remainingLaps != SHRT_MAX && perLapConsEst > 0 )
                {
                    float toFinish = remainingLaps * perLapConsEst - remainingFuel;

                    if( toFinish > ir_session.fuelMaxLtr )
                        m_brush->SetColor( warnCol );
                    else 
                        m_brush->SetColor( goodCol );

                    if( useGallons )
                        toFinish *= 0.264172f;
                    swprintf( s, _countof(s), useGallons ? L"%3.1f gl" : L"%3.1f lt", toFinish );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxFuel.x0, m_boxFuel.x1-5, m_boxFuel.y0+m_boxFuel.h*8.4f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                    m_brush->SetColor( textCol );
                }

                // Add
                float add = ir_PitSvFuel.getFloat();
                if( add >= 0 )
                {
                    if( ir_dpFuelFill.getFloat() )
                        m_brush->SetColor( serviceCol );

                    if( useGallons )
                        add *= 0.264172f;
                    swprintf( s, _countof(s), useGallons ? L"%3.1f gl" : L"%3.1f lt", add );
                    m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxFuel.x0, m_boxFuel.x1-5, m_boxFuel.y0+m_boxFuel.h*10.5f/12.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_TRAILING );
                    m_brush->SetColor( textCol );
                }
            }

            // Tires
            {
                const float lf = 100.0f * std::min(std::min( ir_LFwearL.getFloat(), ir_LFwearM.getFloat() ), ir_LFwearR.getFloat() );
                const float rf = 100.0f * std::min(std::min( ir_RFwearL.getFloat(), ir_RFwearM.getFloat() ), ir_RFwearR.getFloat() );
                const float lr = 100.0f * std::min(std::min( ir_LRwearL.getFloat(), ir_LRwearM.getFloat() ), ir_LRwearR.getFloat() );
                const float rr = 100.0f * std::min(std::min( ir_RRwearL.getFloat(), ir_RRwearM.getFloat() ), ir_RRwearR.getFloat() );

                // Left
                if( ir_dpLTireChange.getFloat() )
                    m_brush->SetColor( serviceCol );
                else
                    m_brush->SetColor( textCol );
                swprintf( s, _countof(s), L"%d", (int)(lf+0.5f) );
                m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), m_boxTires.x0+20, m_boxTires.x0+m_boxTires.w/2, m_boxTires.y0+m_boxTires.h*1.0f/3.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                swprintf( s, _countof(s), L"%d", (int)(lr+0.5f) );
                m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), m_boxTires.x0+20, m_boxTires.x0+m_boxTires.w/2, m_boxTires.y0+m_boxTires.h*2.0f/3.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

                // Right
                if( ir_dpRTireChange.getFloat() )
                    m_brush->SetColor( serviceCol );
                else
                    m_brush->SetColor( textCol );
                swprintf( s, _countof(s), L"%d", (int)(rf+0.5f) );
                m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), m_boxTires.x0+m_boxTires.w/2, m_boxTires.x1-20, m_boxTires.y0+m_boxTires.h*1.0f/3.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
                swprintf( s, _countof(s), L"%d", (int)(rr+0.5f) );
                m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), m_boxTires.x0+m_boxTires.w/2, m_boxTires.x1-20, m_boxTires.y0+m_boxTires.h*2.0f/3.0f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );

                m_brush->SetColor( textCol );
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
                else if( ir_LapDeltaToSessionBestLap_OK.getBool() )
                {
                    t = ir_LapDeltaToSessionBestLap.getFloat();
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
                    m_brush->SetColor( t <= 0 ? goodCol : badCol );
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
                    swprintf( s, _countof(s), L"%d:%02d:%02d", hours, mins, secs );
                else
                    swprintf( s, _countof(s), L"%02d:%02d", mins, secs );
                m_text.render( m_renderTarget.Get(), s, m_textFormatSmall.Get(), m_boxSession.x0, m_boxSession.x1, m_boxSession.y0+m_boxSession.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
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
                swprintf( s, _countof(s), L"%+3.1f", bias );
                m_text.render( m_renderTarget.Get(), s, m_textFormat.Get(), m_boxBias.x0, m_boxBias.x1, m_boxBias.y0+m_boxBias.h*0.5f, m_brush.Get(), DWRITE_TEXT_ALIGNMENT_CENTER );
            }

            // Draw all the box outlines and titles
            m_brush->SetColor( outlineCol );
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

        Box makeBox( float x0, float w, float y0, float h, const std::string& title )
        {
            Box r;

            if( w <= 0 || h <= 0 )
                return r;

            r.x0 = r2ax( x0 );
            r.x1 = r2ax( x0+w );
            r.y0 = r2ay( y0 );
            r.y1 = r2ay( y0+h );
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
        Microsoft::WRL::ComPtr<IDWriteTextFormat>  m_textFormatGear;
        Microsoft::WRL::ComPtr<ID2D1PathGeometry1> m_pathGeometry;

        TextCache    m_text;

        int     m_prevCurrentLap = 0;
        DWORD   m_lastLapChangeTickCount = 0;

        float   m_prevDelta = 0;
        bool    m_prevDeltaOk = false;

        float   m_lapStartRemainingFuel = 0;
        float   m_maxPerLapFuelUsed = 0;
};

