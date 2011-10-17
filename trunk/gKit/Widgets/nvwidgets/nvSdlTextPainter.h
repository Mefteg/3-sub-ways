
// mailto:jean-claude.iehl@liris.cnrs.fr
// provides SDL_ttf text rendering for nvWidgets

#ifndef SDL_TEXT_PAINTER_H
#define SDL_TEXT_PAINTER_H

#include <string>

#include <GL/glew.h>
#ifdef __APPLE_CC__
#include <OpenGL/gl.h>
#include <SDL_ttf/SDL_ttf.h>
#else
#include <GL/gl.h>
#include <SDL/SDL_ttf.h>
#endif

#include <SDL/SDL.h>

#include "nvTextPainter.h"


namespace nv {

struct SdlTextGlyph
{
    float tex_xmin, tex_xmax;
    float tex_ymin, tex_ymax;
    int xmin, xmax;
    int ymin, ymax;
    int advance;
};

class SdlTextPainter : public TextPainter
{
public:
    
    std::string m_name;
    int m_point_size;
    int m_line_skip;
    int m_height;
    int m_ascent;
    bool m_is_init;

    TTF_Font *m_font;
    GLuint m_texture;
    SDL_Surface *m_surface;
    SdlTextGlyph m_cache[128];
    
    void draw_glyph( const int x, const int y, const int id ) const;
    
public:
    SdlTextPainter( const std::string& filename, const int size );
    ~SdlTextPainter( );
    
    void init( );
    
    int getFontHeight( ) const;
    int getTextLineWidth( const char * text ) const;
    int getTextLineWidthAt( const char * text, int charNb ) const;
    int getTextSize( const char * text, int& nbLines ) const;
    int getPickedCharNb( const char * text, const nv::Point& at ) const;
    
    void drawCharacter( int x, int y, const char c ) const;
    void drawString( int x, int y, const char *c, int nbLines ) const;
};

}

#endif
