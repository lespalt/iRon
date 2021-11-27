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
#include <thread>
#include <atomic>
#include <Windows.h>
#include "iracing.h"
#include "Config.h"
#include "OverlayRelative.h"
#include "OverlayInputs.h"

Config              g_cfg;
std::atomic<bool>   g_reloadConfigNeeded = false;

static void configWatcher()
{
    HANDLE dir = CreateFile( "config", FILE_LIST_DIRECTORY, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
    if( dir == INVALID_HANDLE_VALUE )
    {
        printf( "Could not start config watch thread.\n" );
        return;
    }

    std::vector<DWORD> buf( 1024*1024 );
    DWORD bytesReturned = 0;

    while( true )
    {        
        if( ReadDirectoryChangesW( dir, buf.data(), (DWORD)buf.size()/sizeof(DWORD), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL ) )
        {
            Sleep( 100 );  // wait a bit to make sure changes are actually picked up when we reload
            g_reloadConfigNeeded = true;
        }
    }
}

static void handleConfigChange( std::vector<Overlay*> overlays )
{
    for( Overlay* o : overlays )
    {
        o->enable( g_cfg.getBool(o->getName(),"enabled") );
        o->notifyConfigChanged();
    }
}

int main()
{
    // Bump priority up so we get time from the sim
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);  

    // Load the config and watch it for changes
    g_cfg.load();
    std::thread configWatchThread( configWatcher );
    configWatchThread.detach();

    std::vector<Overlay*> overlays;
    overlays.push_back( new OverlayRelative() );
    overlays.push_back( new OverlayInputs() );

    // Initialize by triggering all config handling logic
    handleConfigChange( overlays );

    bool connected = false;
    while( true )
    {
        bool prevConnected = connected;
        connected = ir_tick();
        if( connected != prevConnected )
            printf( connected ? "iRacing: connected\n" : "iRacing: disconnected\n" );

        // Update roughly every 16ms
        for( Overlay* o : overlays )
            o->update();

        // Watch for config change signal
        if( g_reloadConfigNeeded )
        {
            g_cfg.load();
            handleConfigChange( overlays );
            g_reloadConfigNeeded = false;
        }

        // Message pump
        MSG msg = {};
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    for( Overlay* o : overlays )
        delete o;
}
