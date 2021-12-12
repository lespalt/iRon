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
#include "Font.h"

class OverlayRelative : public Overlay
{
    public:

        OverlayRelative()
            : Overlay("OverlayRelative")
        {}

    protected:

        virtual void onEnable()
        {
            m_fnt.load("arial-black");
        }

        virtual void onDisable()
        {
            m_fnt.unload();
        }

        virtual void onUpdate()
        {
            // Clear background
            float4 bgcol = g_cfg.getFloat4( m_name, "background_col" );
            glClearColor( bgcol.r, bgcol.g, bgcol.b, bgcol.a );
            glClear(GL_COLOR_BUFFER_BIT);
            
            struct CarIdx {
                int carIdx;
                float delta;
            };
            std::vector<CarIdx> relatives;
            relatives.reserve( IR_MAX_CARS );

            // Populate cars with the ones for which a relative/delta comparison is valid
            for( int i=0; i<IR_MAX_CARS; ++i )
            {
                if( i == ir_session.driverCarIdx || ir_CarIdxLap.getInt(i) >= 0 )
                {
                    CarIdx ci;
                    ci.carIdx = i;
                    ci.delta = 12.3f;
                    relatives.push_back( ci );
                }
            }

            // Sort by delta
            std::sort( relatives.begin(), relatives.end(), 
                []( const CarIdx& a, const CarIdx&b ) { return a.delta < b.delta; } );

            // Locate our driver's index in the new array
            int driverCarIdx = -1;
            for( int i=0; i<(int)relatives.size(); ++i )
            {
                if( relatives[i].carIdx == ir_session.driverCarIdx ) {
                    driverCarIdx = i;
                    break;
                }
            }

            // Something's wrong if we didn't find our driver. Bail.
            if( driverCarIdx < 0 )
                return;

            const float fontSize = 64;

            // Display such that our driver is in the vertical center of the are where we're listing cars
            const float carAreaTop = m_height - 40.0f;
            const float carAreaBot = 40.0f;
            const float ystart = (carAreaTop-carAreaBot) / 2.0f + fontSize / 2.0f;

            float y = ystart;
            for( int i=driverCarIdx; i>=0 && y<=carAreaTop-fontSize; --i, y+=fontSize )
            {
                const Car& car = ir_session.cars[relatives[i].carIdx];
                m_fnt.render( car.userName, 0, y, fontSize, float4(1,1,1,1) );
            }

            y = ystart - fontSize;
            for( int i=driverCarIdx+1; i<(int)relatives.size() && y>=carAreaBot; ++i, y-=fontSize )
            {
                const Car& car = ir_session.cars[relatives[i].carIdx];
                m_fnt.render( car.userName, 0, y, fontSize, float4(1,1,1,1) );
            }
        }

    protected:

        Font    m_fnt;
};
