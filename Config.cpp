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


#include <atomic>
#include "Config.h"

Config              g_cfg;

static void configWatcher( std::atomic<bool>* m_hasChanged )
{
    HANDLE dir = CreateFile( ".", FILE_LIST_DIRECTORY, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
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
            *m_hasChanged = true;
        }
    }
}

bool Config::load()
{
    std::string json;
    if( !loadFile(m_filename, json) )
    {
        printf("Could not load config file\n");
        return false;
    }

    picojson::value pjval;
    std::string parseError = picojson::parse( pjval, json );
    if( !parseError.empty() )
    {
        printf("Config file is not valid JSON!\n%s\n", parseError.c_str() );
        return false;
    }

    m_pj = pjval.get<picojson::object>();
    m_hasChanged = false;
    return true;
}

bool Config::save()
{
    const picojson::value value = picojson::value( m_pj );
    const std::string json = value.serialize(true);
    return saveFile( m_filename, json );
}

void Config::watchForChanges()
{
    m_configWatchThread = std::thread( configWatcher, &m_hasChanged );
    m_configWatchThread.detach();
}

bool Config::hasChanged()
{
    return m_hasChanged;
}

bool Config::getBool( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    return pjcomp[key].get<bool>();
}

int Config::getInt( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    return (int)pjcomp[key].get<double>();
}

float Config::getFloat( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    return (float)pjcomp[key].get<double>();
}

float4 Config::getFloat4( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    picojson::array& arr = pjcomp[key].get<picojson::array>();
    float4 ret;
    ret.x = (float)arr[0].get<double>();
    ret.y = (float)arr[1].get<double>();
    ret.z = (float)arr[2].get<double>();
    ret.w = (float)arr[3].get<double>();
    return ret;
}

std::string Config::getString( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    return pjcomp[key].get<std::string>();
}

std::vector<std::string> Config::getStringVec( const std::string& component, const std::string& key )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    picojson::array& arr = pjcomp[key].get<picojson::array>();
    std::vector<std::string> ret;
    for( picojson::value& val : arr )
        ret.push_back( val.get<std::string>() );
    return ret;
}

void Config::setInt( const std::string& component, const std::string& key, int v )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    double d = double(v);
    pjcomp[key].set<double>( d );
}
