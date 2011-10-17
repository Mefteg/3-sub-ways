
#include <cassert>
#include <ctype.h>
#include <cmath>

#include "nvSdlTextPainter.h"


namespace nv {
    
SdlTextPainter::SdlTextPainter( const std::string& filename, const int size )
    :
    m_name(filename),
    m_point_size(size),
    m_is_init(false),
    m_font(NULL)
{}

SdlTextPainter::~SdlTextPainter( )
{
    if(m_font != NULL)
        TTF_CloseFont(m_font);
    if(m_texture != 0)
        glDeleteTextures(1, &m_texture);
    if(m_surface != NULL)
        SDL_FreeSurface(m_surface);
    
    TTF_Quit();
#ifdef VERBOSE
    printf("SdlTextPainter( ): quit.\n");
#endif
}

int power2( const int value )
{
    int p= 1;
    while(p < value)
        p+= p;
    
    return p;
}

void SdlTextPainter::init( )
{
    if(m_is_init == true)
        return;
    
    if(TTF_WasInit() == 0)
        TTF_Init();
    assert(TTF_WasInit() != 0);
    
    // open font
    m_font= TTF_OpenFont(m_name.c_str(), m_point_size);
    if(m_font == NULL)
    {
        printf(" -- error readinf ttf font '%s': %s\n", m_name.c_str(), TTF_GetError());
        return;
    }

    printf("loading ttf font '%s'...\n", TTF_FontFaceFamilyName(m_font));

#if 0    
    // disable kerning, ttf >= 2.0.10
    TTF_SetFontKerning(m_font, 0);
    
    // ttf version >= 2.0.10
    int hinting= TTF_GetFontHinting(font);
    printf("  hinting %s\n",
        hinting == 0 ? "Normal" :
        hinting== 1 ? "Light" :
        hinting == 2 ? "Mono" :
        hinting == 3 ? "None" :
        "??");
#endif
    
    // get metrics
    m_ascent= TTF_FontAscent(m_font);
    m_line_skip= TTF_FontLineSkip(m_font);
    m_height= TTF_FontHeight(m_font);
    
    // fills the glyphs cache, replaces non printable ascii characters with a '?'
    // 1. compute texture area
    int area= 0;
    SDL_Color color= { 255, 255, 255, 0 };
    for(int i= 0; i < 128; i++)
    {
        const int g= isprint(i) ? i : '?';
        
        TTF_GlyphMetrics(m_font, g, 
            &m_cache[i].xmin, &m_cache[i].xmax,
            &m_cache[i].ymin, &m_cache[i].ymax,
            &m_cache[i].advance);
        
        SDL_Surface *glyph= TTF_RenderGlyph_Blended(m_font, g, color);
        area+= glyph->w * glyph->h;
        
        SDL_FreeSurface(glyph);
    }
    
    // 2. pick up a texture size
    const int w= power2((int) (sqrtf(2.f * area) + .5f));
    const int h= w;
    
#ifdef VERBOSE_DEBUG
    printf("texture %dx%d (area %d > glyphs area %d)\n", w, h, w*h, area);
#endif
    
    m_surface= SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
    #else
        0xFF000000,
        0x00FF0000,
        0x0000FF00,
        0x000000FF
    #endif
        );
    assert(m_surface != NULL);

    // 3. packing
    int x= 0;
    int y= 0;
    for(int i= 0; i < 128; i++)
    {
        const int g= isprint(i) ? i : '?';
        SDL_Surface *glyph= TTF_RenderGlyph_Blended(m_font, g, color);
        
        // place current glyph
        if(x + glyph->w > w)
        {
            y+= m_height;
            x= 0;
        }
        assert(y < h);
        
        // texcoords
        m_cache[i].tex_xmin= (float) x / (float) w;
        m_cache[i].tex_xmax= (float) (x + glyph->w) / (float) w;
        // flip texture ...
        m_cache[i].tex_ymax= (float) y / (float) h;
        m_cache[i].tex_ymin= (float) (y + glyph->h) / (float) h;
        
        // blit glyph 
        SDL_Rect surface_area;
        surface_area.x= x;
        surface_area.y= y;
        surface_area.w= glyph->w;
        surface_area.h= glyph->h;
        
        SDL_Rect glyph_area;
        glyph_area.x= 0;
        glyph_area.y= 0;
        glyph_area.w= glyph->w;
        glyph_area.h= glyph->h;
        
        // magic from slouken, cf. http://www.libsdl.org/cgi/viewvc.cgi?view=revision&revision=4463
        if(glyph->flags & SDL_SRCALPHA)
            SDL_SetAlpha(glyph, 0, 0); 
        SDL_BlitSurface(glyph, &glyph_area, m_surface, &surface_area);
        
        // place next glyph
        x+= glyph->w +1;
        SDL_FreeSurface(glyph);
    }
    
    // 4. build an opengl texture
    glGenTextures(1, &m_texture);
    assert(m_texture != 0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 
        0, GL_RGBA, w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, m_surface->pixels);
    
    m_is_init= true;
}


int SdlTextPainter::getFontHeight( ) const
{
    return m_line_skip;
}

int SdlTextPainter::getTextLineWidth( const char * text ) const
{
    int w= 0;
    for(int i= 0; text[i] != 0 && text[i] != '\n'; i++)
    {
        const int g= ((unsigned char) text[i] < 128) ? (unsigned char) text[i] : '?';
        w+= m_cache[g].advance;
    }
    
    return w +2;
}

int SdlTextPainter::getTextLineWidthAt( const char * text, int pos ) const
{
    int w= 0;
    for(int i= 0; i < pos && text[i] != 0 && text[i] != '\n'; i++)
    {
        const int g= ((unsigned char) text[i] < 128) ? (unsigned char) text[i] : '?';
        w+= m_cache[g].advance;
    }
    
    return w +2;
}

int SdlTextPainter::getTextSize( const char * text, int& nbLines ) const
{
    int n= 1;
    int w= 0;
    int wmax= 0;
    for(int i= 0; text[i] != 0; i++)
    {
        if(text[i] == '\n')
        {
            n+= 1;
            if(w > wmax)
                wmax= w;
            w= 0;
            continue;
        }
        
        const int g= ((unsigned char) text[i] < 128) ? (unsigned char) text[i] : '?';
        w+= m_cache[g].advance;
    }

    if(w > wmax)
        wmax= w;
    nbLines= n;
    return wmax +2;
}

int SdlTextPainter::getPickedCharNb( const char * text, const nv::Point& at ) const
{
    int i;
    int w= 0;
    for(i= 0; text[i] != 0 && text[i] != '\n'; i++)
    {
        if(at.x < w)
            return i;
        
        const int g= ((unsigned char) text[i] < 128) ? (unsigned char) text[i] : '?';
        w+= m_cache[g].advance;
    }
    
    return i;
}

void SdlTextPainter::drawCharacter( int x, int y, const char c ) const
{
    return;
}

void SdlTextPainter::draw_glyph( const int x, const int y, const int id ) const
{
    assert(id >= 0 && id < 128);
    
    // align top left corner 
    // cf. http://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_38.html#SEC38
    const float yy= y +2;
    
    glTexCoord2f(m_cache[id].tex_xmin, m_cache[id].tex_ymin);
    glVertex2f(x + m_cache[id].xmin, yy + m_cache[id].ymin);
    
    glTexCoord2f(m_cache[id].tex_xmax, m_cache[id].tex_ymin);
    glVertex2f(x + m_cache[id].xmax, yy + m_cache[id].ymin);
    
    glTexCoord2f(m_cache[id].tex_xmax, m_cache[id].tex_ymax);
    glVertex2f(x + m_cache[id].xmax, yy + m_cache[id].ymax);
    
    glTexCoord2f(m_cache[id].tex_xmin, m_cache[id].tex_ymax);
    glVertex2f(x + m_cache[id].xmin, yy + m_cache[id].ymax);
}

void SdlTextPainter::drawString( int x, int y, const char *text, int lines ) const
{
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindSampler(0, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBegin(GL_QUADS);
    
    int w= 0;
    int n= 0;
    for(int i= 0; n < lines && text[i] != 0; i++)
    {
        if(text[i] == '\n')
        {
            n+= 1;
            w= 0;
            continue;
        }
        
        const int g= ((unsigned char) text[i] < 128) ? (unsigned char) text[i] : '?';
        draw_glyph(x + w, y + m_line_skip * n, g);
        
        w+= m_cache[g].advance;
    }
    
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
