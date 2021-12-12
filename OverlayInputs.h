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

    protected:

        virtual void onConfigChanged()
        {
            // Width might have changed, reset tracker values
            m_throttleVtx.resize( m_width );
            m_brakeVtx.resize( m_width );
            m_steerVtx.resize( m_width );
            for( int i=0; i<m_width; ++i )
            {
                m_throttleVtx[i].x = float(i);
                m_brakeVtx[i].x = float(i);
                m_steerVtx[i].x = float(i);
            }
        }

        virtual void onUpdate()
        {
            const float w = (float)m_width;
            const float h = (float)m_height;

            // Advance input vertices
            for( int i=0; i<(int)m_throttleVtx.size()-1; ++i )
                m_throttleVtx[i].y = m_throttleVtx[i+1].y;
            m_throttleVtx[(int)m_throttleVtx.size()-1].y = ir_Throttle.getFloat();

            for( int i=0; i<(int)m_brakeVtx.size()-1; ++i )
                m_brakeVtx[i].y = m_brakeVtx[i+1].y;
            m_brakeVtx[(int)m_brakeVtx.size()-1].y = ir_Brake.getFloat();

            for( int i=0; i<(int)m_steerVtx.size()-1; ++i )
                m_steerVtx[i].y = m_steerVtx[i+1].y;
            m_steerVtx[(int)m_steerVtx.size()-1].y = std::min( 1.0f, std::max( 0.0f, (ir_SteeringWheelAngle.getFloat() / ir_SteeringWheelAngleMax.getFloat()) * -0.5f + 0.5f) );

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
                glVertex2f(m_throttleVtx[i].x,m_throttleVtx[i].y*(h-1));
            glEnd();

            // Brake line
            col = g_cfg.getFloat4( m_name, "brake_col" );
            glColor4fv( &col );
            glBegin( GL_LINE_STRIP );
            for( int i=0; i<(int)m_brakeVtx.size(); ++i )
                glVertex2f(m_brakeVtx[i].x,m_brakeVtx[i].y*(h-1));
            glEnd();

            // Steering line
            col = g_cfg.getFloat4( m_name, "steering_col" );
            glColor4fv( &col );
            glBegin( GL_LINE_STRIP );
            for( int i=0; i<(int)m_steerVtx.size(); ++i )
                glVertex2f(m_steerVtx[i].x,m_steerVtx[i].y*(h-1));
            glEnd();
        }

    protected:

        std::vector<float2> m_throttleVtx;
        std::vector<float2> m_brakeVtx;
        std::vector<float2> m_steerVtx;
};
