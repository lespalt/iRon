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


#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>
#include "iracing.h"
#include "Config.h"
#include "OverlayRelative.h"
#include "OverlayInputs.h"


static void handleConfigChange( std::vector<Overlay*> overlays )
{
    for( Overlay* o : overlays )
    {
        o->enable( g_cfg.getBool(o->getName(),"enabled") );
        o->configChanged();
    }
}

int main()
{
    // Bump priority up so we get time from the sim
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    // Load the config and watch it for changes
    g_cfg.load();
    g_cfg.watchForChanges();

    // Register hotkey to enable/disable position/size changes.
    const int hotkey = g_cfg.getString( "General", "ui_edit_hotkey_is_alt_and_this_letter" )[0];
    RegisterHotKey( NULL, 0, MOD_ALT, toupper(hotkey) );

    // Create and initialize overlays by triggering all config handling logic
    std::vector<Overlay*> overlays;
    overlays.push_back( new OverlayRelative() );
    overlays.push_back( new OverlayInputs() );
    handleConfigChange( overlays );

    bool connected = false;
    bool uiEdit = false;

    while( true )
    {
        bool prevConnected = connected;
        connected = ir_tick();
        if( connected != prevConnected )
        {
            printf( connected ? "iRacing: connected\n" : "iRacing: disconnected\n" );
        }

        // Update roughly every 16ms
        for( Overlay* o : overlays )
            o->update();

        // Watch for config change signal
        if( g_cfg.hasChanged() )
        {
            g_cfg.load();
            handleConfigChange( overlays );
        }

        // Message pump
        MSG msg = {};
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if( msg.message == WM_HOTKEY )
            {
                uiEdit = !uiEdit;
                for( Overlay* o : overlays )
                    o->enableUiEdit( uiEdit );
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    for( Overlay* o : overlays )
        delete o;
}
