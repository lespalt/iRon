
#include <atomic>
#include "Config.h"

Config              g_cfg;

static void configWatcher( std::atomic<bool>* m_hasChanged )
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
            *m_hasChanged = true;
        }
    }
}

bool Config::load()
{
    std::string json;
    if( !loadFile("config/config.json", json) )
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

bool Config::loadFile( const char* fname, std::string& output )
{
    FILE* fp = fopen( fname, "rb" );
    if( !fp )
        return false;

    fseek( fp, 0, SEEK_END );
    const long sz = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    char* buf = new char[sz];

    fread( buf, 1, sz, fp );
    fclose( fp );
    output = std::string( buf, sz );

    delete[] buf;
    return true;
}
