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
#include <atomic>
#include <thread>
#include <vector>
#include "picojson.h"
#include "util.h"

class Config
{
    public:

        bool    load();
        bool    save();

        void    watchForChanges();
        bool    hasChanged();

        bool                     getBool( const std::string& component, const std::string& key );
        int                      getInt( const std::string& component, const std::string& key );
        float                    getFloat( const std::string& component, const std::string& key );
        float4                   getFloat4( const std::string& component, const std::string& key );
        std::string              getString( const std::string& component, const std::string& key );
        std::vector<std::string> getStringVec( const std::string& component, const std::string& key );

        void                     setInt( const std::string& component, const std::string& key, int v );
        void                     setBool( const std::string& component, const std::string& key, bool v );

    private:

        picojson::object    m_pj;
        std::atomic<bool>   m_hasChanged = false;
        std::thread         m_configWatchThread;
        std::string         m_filename = "config.json";
};

extern Config        g_cfg;

