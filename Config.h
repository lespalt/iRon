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

#include "picojson.h"

struct float2
{
    union { float r; float x; };
    union { float g; float y; };
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

struct float4
{
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    union { float a; float w; };
    float* operator&() { return &x; }
    const float* operator&() const { return &x; }
};

class Config
{
    public:

        void load()
        {
            std::string json;
            if( !loadFile("config/config.json", json) )
            {
                printf("Could not load config file\n");
                return;
            }

            picojson::value pjval;
            std::string parseError = picojson::parse( pjval, json );
            if( !parseError.empty() )
            {
                printf("Config file is not valid JSON!\n%s\n", parseError.c_str() );
                return;
            }

            m_pj = pjval.get<picojson::object>();
        }

        bool getBool( const std::string& component, const std::string& key )
        {
            picojson::object& pjcomp = m_pj[component].get<picojson::object>();
            return pjcomp[key].get<bool>();
        }

        int getInt( const std::string& component, const std::string& key )
        {
            picojson::object& pjcomp = m_pj[component].get<picojson::object>();
            return (int)pjcomp[key].get<double>();
        }

        float getFloat( const std::string& component, const std::string& key )
        {
            picojson::object& pjcomp = m_pj[component].get<picojson::object>();
            return (float)pjcomp[key].get<double>();
        }

        float4 getFloat4( const std::string& component, const std::string& key )
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


    private:

        bool loadFile( const char* fname, std::string& output )
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

        picojson::object  m_pj;
};

extern Config g_cfg;