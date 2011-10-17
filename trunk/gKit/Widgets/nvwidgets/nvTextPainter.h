
// TextPainter.h - User Interface library
// abstracting font rendering from glutBitmap*( ) calls

// mailto:jean-claude.iehl@liris.cnrs.fr
    
#ifndef NV_TEXT_PAINTER_H
#define NV_TEXT_PAINTER_H

#include "nvWidgets.h"


namespace nv {

class TextPainter {
public:
    TextPainter( ) {}
    ~TextPainter( ) {}

    virtual void init( ) = 0;
    
    virtual int getFontHeight( ) const = 0;
    virtual int getTextLineWidth( const char * text ) const = 0;
    virtual int getTextLineWidthAt( const char * text, int charNb ) const = 0;
    virtual int getTextSize( const char * text, int& nbLines ) const = 0;
    virtual int getPickedCharNb( const char * text, const Point& at ) const = 0;
    
    virtual void drawCharacter( int x, int y, const char c ) const = 0;
    virtual void drawString( int x, int y, const char *c, int nbLines ) const = 0;
};

}

#endif
