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

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "lodepng/lodepng.h"
#include "util.h"


//
// Rendering of fonts exported with BMFont (AngelCode.com).
// Make sure to set 'Bit depth' to 32 in BMFont's export options in order
// to get alpha-antialiased outlines.
//
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
            if( lodepng_decode_file( &pngBuf, &width, &height, m_pngFilename.c_str(), LCT_RGBA, 8 ) )
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
            glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_common.scaleW, m_common.scaleH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pngBuf );
            free( pngBuf );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glDisable( GL_TEXTURE_2D );

            return true;
        }

        void unload()
        {
            if( !isLoaded() )
                return;

            glDeleteTextures( 1, &m_fntTex );
            m_fntTex = 0;
            m_common = Common();
            m_charDescs.clear();
            m_kernings.clear();
        }

        void render( const std::string& text, float x, float y, float lineheight, const float4& color )
        {
            const float scale = lineheight / m_common.lineHeight;

            glColor4fv( &color );
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, m_fntTex );

            int prevId = -1;

            glBegin( GL_QUADS );            
            for( int i=0; i<(int)text.length(); ++i )
            {
                const int id = (int)(unsigned char)text[i];
                const CharDesc& cd = m_charDescs[id];

                int kerning = 0;
                if( i > 0 ) {
                    auto it = m_kernings.find(std::make_pair(prevId,id));
                    if( it != m_kernings.end() )
                        kerning = it->second;
                }

                const float left     = 0.5f + x + cd.xoffset * scale;
                const float right    = left + cd.width * scale;
                const float top      = 0.5f + y + float(m_common.lineHeight-cd.yoffset) * scale;
                const float bottom   = 0.5f + y + float(m_common.lineHeight-cd.yoffset-cd.height) * scale;

                const float tcleft   = (0.5f+cd.x)            / float(m_common.scaleW);
                const float tcright  = (0.5f+cd.x+cd.width)   / float(m_common.scaleW);
                const float tctop    = (0.5f+cd.y)            / float(m_common.scaleH);
                const float tcbottom = (0.5f+cd.y+cd.height)  / float(m_common.scaleH);

                glTexCoord2f( tcleft, tctop );
                glVertex2f( left, top );

                glTexCoord2f( tcleft, tcbottom );
                glVertex2f( left, bottom );

                glTexCoord2f( tcright, tcbottom );
                glVertex2f( right, bottom );

                glTexCoord2f( tcright, tctop );
                glVertex2f( right, top );
                
                prevId = id;
                x += float(cd.xadvance+kerning) * scale;
            }
            glEnd();

            glDisable( GL_TEXTURE_2D );
            glColor4f( 1, 1, 1, 1 );
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
                m_charDescs.insert( std::make_pair(cd.id,cd) );
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
        std::map<int,CharDesc>              m_charDescs;
        std::map<std::pair<int,int>,int>    m_kernings;
        std::string                         m_pngFilename;
        GLuint                              m_fntTex = 0;
};
