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
        //printf("Could not load config file\n");
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
    const bool ok = saveFile( m_filename, json );
    if( !ok ) {
        char s[1024];
        GetCurrentDirectory( sizeof(s), s );
        printf("Could not save config file! Please make sure iRon is started from a directory for which it has write permissions. The current directory is: %s.\n", s);
    }
    return ok;
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

bool Config::getBool( const std::string& component, const std::string& key, bool defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
        value.set<bool>( defaultVal );

    return value.get<bool>();
}

int Config::getInt( const std::string& component, const std::string& key, int defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
        value.set<double>( defaultVal );

    return (int)value.get<double>();
}

float Config::getFloat( const std::string& component, const std::string& key, float defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
        value.set<double>( defaultVal );

    return (float)value.get<double>();
}

float4 Config::getFloat4( const std::string& component, const std::string& key, const float4& defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
    {
        picojson::array arr( 4 );
        arr[0].set<double>( defaultVal.x );
        arr[1].set<double>( defaultVal.y );
        arr[2].set<double>( defaultVal.z );
        arr[3].set<double>( defaultVal.w );
        value.set<picojson::array>( arr );
    }

    picojson::array& arr = value.get<picojson::array>();
    float4 ret;
    ret.x = (float)arr[0].get<double>();
    ret.y = (float)arr[1].get<double>();
    ret.z = (float)arr[2].get<double>();
    ret.w = (float)arr[3].get<double>();
    return ret;
}

std::string Config::getString( const std::string& component, const std::string& key, const std::string& defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
        value.set<std::string>( defaultVal );

    return value.get<std::string>();
}

std::vector<std::string> Config::getStringVec( const std::string& component, const std::string& key, const std::vector<std::string>& defaultVal )
{
    bool existed = false;
    picojson::value& value = getOrInsertValue( component, key, &existed );

    if( !existed )
    {
        picojson::array arr( defaultVal.size() );
        for( int i=0; i<(int)defaultVal.size(); ++i )
            arr[i].set<std::string>( defaultVal[i] );
        value.set<picojson::array>( arr );
    }

    picojson::array& arr = value.get<picojson::array>();
    std::vector<std::string> ret( arr.size() );
    for( picojson::value& entry : arr )
        ret.push_back( entry.get<std::string>() );
    return ret;
}

void Config::setInt( const std::string& component, const std::string& key, int v )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    double d = double(v);
    pjcomp[key].set<double>( d );
}

void Config::setBool( const std::string& component, const std::string& key, bool v )
{
    picojson::object& pjcomp = m_pj[component].get<picojson::object>();
    pjcomp[key].set<bool>( v );
}

picojson::object& Config::getOrInsertComponent( const std::string& component, bool* existed )
{
    auto it = m_pj.insert(std::make_pair(component,picojson::object()));
    
    if( existed )
        *existed = !it.second;

    return it.first->second.get<picojson::object>();
}

picojson::value& Config::getOrInsertValue( const std::string& component, const std::string& key, bool* existed )
{
    picojson::object& comp = getOrInsertComponent( component );

    auto it = comp.insert(std::make_pair(key,picojson::value()));

    if( existed )
        *existed = !it.second;

    return it.first->second;
}
