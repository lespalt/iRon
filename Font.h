#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "lodepng/lodepng.h"

// Rendering of fonts exported with BMFont (AngelCode.com)
class Font
{
    public:        

        bool isLoaded() const
        {
            return m_fntTex != 0;
        }

        bool load( const std::string& name )
        {
            if( isLoaded() )
                return true;

            if( !loadFntDesc(name) )
                return false;

            unsigned char* pngBuf = nullptr;
            unsigned width=0, height=0;
            if( lodepng_decode32_file( &pngBuf, &width, &height, m_pngFilename.c_str() ) )
            {
                printf( "Error loading %s\n", m_pngFilename.c_str() );
                return false;
            }
            if( width != m_common.scaleW || height != m_common.scaleH )
            {
                printf( "Error: font texture dimensions don't match expectations\n" );
                return false;
            }

            glEnable( GL_TEXTURE_2D );
            glGenTextures( 1, &m_fntTex );
            glBindTexture( GL_TEXTURE_2D, m_fntTex );
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_common.scaleW, m_common.scaleH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pngBuf );
            free( pngBuf );
            glDisable( GL_TEXTURE_2D );

            return true;
        }

        void unload()
        {
            if( !isLoaded() )
                return;

            glDeleteTextures( 1, &m_fntTex );
            m_common = Common();
            m_charDescs.clear();
            m_kernings.clear();
        }

    private:

        bool loadFntDesc( const std::string& name )
        {
            FILE* fpfnt = fopen( (name+".fnt").c_str(), "rb" );
            if( !fpfnt )
            {
                printf( "Error opening %s.fnt\n", name.c_str() );
                return false;
            }

            char s[1024];
            fgets( s, sizeof(s), fpfnt );
            fgets( s, sizeof(s), fpfnt );
            if( sscanf( s, "common lineHeight=%d base=%d scaleW=%d scaleH=%d pages=%d packed=%d alphaChnl=%d redChnl=%d greenChnl=%d blueChnl=%d\n",
                &m_common.lineHeight, &m_common.base, &m_common.scaleW, &m_common.scaleH, &m_common.pages, &m_common.packed,
                &m_common.alphaChnl, &m_common.redChnl, &m_common.greenChnl, &m_common.blueChnl )
                != 10 )
            {
                printf( "Unexpected format of %s.fnt\n", name.c_str() );
                fclose( fpfnt );
                return false;
            }

            char pngFilename[256];
            fgets( s, sizeof(s), fpfnt );
            sscanf( s, "page id=0 file=\"%s\"\n", pngFilename );
            pngFilename[strlen(pngFilename)-1] = '\0'; // kill trailing quotes
            m_pngFilename = pngFilename;

            int charsCount = 0;
            fgets( s, sizeof(s), fpfnt );
            sscanf( s, "chars count=%d", &charsCount );

            for( int i=0; i<charsCount; ++i )
            {
                CharDesc cd;
                fgets( s, sizeof(s), fpfnt );
                if( sscanf( s, "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%d chnl=%d\n",
                    &cd.id, &cd.x, &cd.y, &cd.width, &cd.height, &cd.xoffset, &cd.yoffset, &cd.xadvance, &cd.page, &cd.chnl )
                    != 10 )
                {
                    printf( "Unexpected format of %s.fnt\n", name.c_str() );
                    fclose( fpfnt );
                    return false;
                }
                m_charDescs.push_back( cd );
            }

            int kerningsCount = 0;
            fgets( s, sizeof(s), fpfnt );
            sscanf( s, "kernings count=%d", &kerningsCount );

            for( int i=0; i<kerningsCount; ++i )
            {
                int first=0, second=0, amount=0;
                fgets( s, sizeof(s), fpfnt );
                if( sscanf( s, "kerning first=%d second=%d amount=%d\n", &first, &second, &amount ) != 3 ) 
                {
                    printf( "Unexpected format of %s.fnt\n", name.c_str() );
                    fclose( fpfnt );
                    return false;
                }
                m_kernings[std::make_pair(first,second)] = amount;
            }

            fclose( fpfnt );
            return true;
        }

        struct Common
        {
            int lineHeight = 0;
            int base = 0;
            int scaleW = 0;
            int scaleH = 0;
            int pages = 0;
            int packed = 0;
            int alphaChnl = 0;
            int redChnl = 0;
            int greenChnl = 0;
            int blueChnl = 0;
        };

        struct CharDesc
        {
            int id = 0;
            int x = 0;
            int y = 0;
            int width = 0;
            int height = 0;
            int xoffset = 0;
            int yoffset = 0;
            int xadvance = 0;
            int page = 0;
            int chnl = 0;
        };

        Common                              m_common;
        std::vector<CharDesc>               m_charDescs;
        std::map<std::pair<int,int>,int>    m_kernings;
        std::string                         m_pngFilename;
        GLuint                              m_fntTex = 0;
};
