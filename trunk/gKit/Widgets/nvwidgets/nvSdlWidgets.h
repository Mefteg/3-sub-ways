
// nvSdlWidgets

//  Adaptor classes to integrate the nvWidgets UI library with SDL
// mailto:jean-claude.iehl@liris.cnrs.fr

#ifndef NV_SDL_WIDGETS_H
#define NV_SDL_WIDGETS_H

#include <vector>

#include "nvGLWidgets.h"
#include "nvSdlTextPainter.h"

namespace nv
{

class NVSDKENTRY SdlUIContext : public UIContext
{
public:

    //
    // Default UI constructor
    //
    //  Creates private OpenGL painter
    //////////////////////////////////////////////////////////////////
    SdlUIContext() :
            UIContext( *(new GLUIPainter( *(new SdlTextPainter( "verdana.ttf", 12 )) ))),
            m_mouseX(0), m_mouseY(0),
            m_mouseButton(0),
            m_ownPainter( true )
    {}
    
    //
    // Alternate UI constructor
    //
    //  Allows for overriding the standard painter
    //////////////////////////////////////////////////////////////////
    SdlUIContext( UIPainter& painter ) :
            UIContext( painter ),
            m_ownPainter( false )
    {}
    
    //
    // UI  destructor
    //
    //  Destroy painter if it is private
    //////////////////////////////////////////////////////////////////
    ~SdlUIContext()
    {
        if ( m_ownPainter ) 
            delete getPainter();
    }
    
    //
    // One time initialization
    //
    //////////////////////////////////////////////////////////////////
    bool init( int w, int h );
    
    //
    // process sdl mouse events
    //
    //////////////////////////////////////////////////////////////////
    void processMouseMotionEvent( SDL_MouseMotionEvent& event );
    void processMouseButtonEvent( SDL_MouseButtonEvent& event );
    
    //
    // process sdl keyboard events
    //
    //////////////////////////////////////////////////////////////////
    void processKeyboardEvent( SDL_KeyboardEvent& event );
    
    //
    // process sdl state 
    //
    //////////////////////////////////////////////////////////////////
    void processState( );
    
private:

    //
    //  Translate non-ascii keys from SDL to nvWidgets
    //////////////////////////////////////////////////////////////////
    unsigned char translateKey( SDLKey k );
    
    int m_mouseX;
    int m_mouseY;
    int m_mouseButton;
    std::vector<unsigned char> m_keys;

    bool m_ownPainter;
};

};

#endif
