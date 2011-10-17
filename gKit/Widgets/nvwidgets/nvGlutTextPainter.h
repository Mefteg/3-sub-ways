
// mailto:jean-claude.iehl@liris.cnrs.fr

#ifndef NV_GLUT_TEXT_PAINTER_H
#define NV_GLUT_TEXT_PAINTER_H

#include "nvTextPainter.h"


namespace nv {

class GlutTextPainter : public TextPainter {
    unsigned int m_textListBase;
    
public:
    GlutTextPainter( ):
        m_textListBase(0)
    {}
    
    ~GlutTextPainter( );

    void init( );
    
    int getFontHeight( ) const;
    int getTextLineWidth( const char * text ) const;
    int getTextLineWidthAt( const char * text, int charNb ) const;
    int getTextSize( const char * text, int& nbLines ) const;
    int getPickedCharNb( const char * text, const Point& at ) const;
    
    void drawCharacter( int x, int y, const char c ) const;
    void drawString( int x, int y, const char * text, int nbLines ) const;
};

}

#endif
