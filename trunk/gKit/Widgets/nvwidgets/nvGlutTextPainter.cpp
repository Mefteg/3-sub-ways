
// mailto:jean-claude.iehl@liris.cnrs.fr
// provides glut text rendering for nvWidgets

// quick and dirty refactoring of glutBitmap*( ) calls from nvGLWidgets.cpp


#include <algorithm>
#include <cstring>

#include <GL/glew.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h> // glutBitmapWidth, glutBitmapCharacter
#endif

#include "nvGlutTextPainter.h"


namespace nv {

void GlutTextPainter::init( ) {
    if(m_textListBase == 0) {
        // just doing 7-bit ascii
        m_textListBase = glGenLists(128);

        for (int ii = 0; ii < 128; ii++) {
            glNewList( m_textListBase + ii, GL_COMPILE);
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, ii);
            glEndList();
        }
    }
}

GlutTextPainter::~GlutTextPainter( ) {
    if(m_textListBase != 0)
        glDeleteLists(m_textListBase, 128);
}

int GlutTextPainter::getFontHeight( ) const {
     return 12 + 4 ;
}

int GlutTextPainter::getTextLineWidth( const char * text ) const {
    int w = 0;
    while (*text != '\0' && *text != '\n') {
        w += glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, *text);
        text++;
    }
    w += 2;
    return w;
}

int GlutTextPainter::getTextLineWidthAt( const char * text, int charNb ) const {
    int w = 1;
    for (int i = 0; i < charNb; i++) {
        if (*text != '\0' && *text != '\n') {
            w += glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, *text);
        } else {
            return w + 1;
        }
        text++;
    }

    return w;
}

int GlutTextPainter::getTextSize( const char * text, int& nbLines ) const {
    int w = 0;
    int wLine = 0;
    nbLines = 1;
    while (*text != '\0') {
        if (*text != '\n') {
            wLine += glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, *text);
        } else {
            nbLines++;
            w = std::max(w, wLine);
            wLine = 0;
        }

        text++;
    }

    w = std::max(w, wLine) + 2;
    return w;
}

int GlutTextPainter::getPickedCharNb( const char * text, const Point& at ) const {
    const char * textstart = text;

    int w = 1;
    if ( at.x < w ) return 0;

    while (*text != '\0' && *text != '\n') {
        w += glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, *text);
        if ( at.x < w ) return (text - textstart);
        text++;
    }

    return (text - textstart);
}

void GlutTextPainter::drawCharacter( int x, int y, const char c ) const {
    glListBase(m_textListBase);
    glRasterPos2i(x + 1, y + 4);
    glCallList((GLsizei) c);
}

void GlutTextPainter::drawString( int x, int y, const char *text, int nbLines ) const {
    glListBase(m_textListBase);

    if (nbLines < 2) {
        glRasterPos2i(x + 1, y + 4);
        glCallLists((GLsizei) strlen(text), GL_UNSIGNED_BYTE, text);
    } else {
        int lineNb = nbLines;
        int startNb = 0;
        int endNb = 0;

        const char* textOri = text;
        while (lineNb) {
            if (*text != '\0' && *text != '\n') {
                endNb++;
            } else {
                lineNb--;
                glRasterPos2i(x + 1, y + 4 + lineNb*getFontHeight());
                glCallLists(endNb - startNb , GL_UNSIGNED_BYTE, textOri + startNb);

                endNb++;
                startNb = endNb;
            }
            text++;
        }
    }
}

};
