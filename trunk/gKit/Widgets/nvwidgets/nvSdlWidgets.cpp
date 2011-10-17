
// nvSdlWidgets

#include <GL/glew.h>

#include "nvSdlWidgets.h"

using namespace nv;

bool SdlUIContext::init(int w, int h) {

    if (glewInit() != GLEW_OK) {
        return false;
    }

    if (!glewIsSupported(
                "GL_VERSION_2_0 "
                "GL_ARB_vertex_program "
                "GL_ARB_fragment_program "
            )) {
        return false;
    }

    reshape(w, h);

    // enable unicode translation of scan codes
    SDL_EnableUNICODE(1);
    
    // copy current keyboard state
    int nkeys;
    Uint8 *keys= SDL_GetKeyState(&nkeys);
    m_keys.reserve(nkeys);
    m_keys.insert(m_keys.begin(), &keys[0], &keys[nkeys]);
    
    return true;
}

void SdlUIContext::processMouseMotionEvent( SDL_MouseMotionEvent& event )
{
    if(event.state != 0)
        UIContext::mouseMotion(event.x, event.y);    
}

void SdlUIContext::processMouseButtonEvent( SDL_MouseButtonEvent& event )
{
    int uiButton= -1;
    if(event.button == SDL_BUTTON_LEFT)
        uiButton= MouseButton_Left;
    else if(event.button == SDL_BUTTON_MIDDLE)
        uiButton= MouseButton_Middle;
    else if(event.button == SDL_BUTTON_RIGHT)
        uiButton= MouseButton_Right;
    else
        // another button event (wheel ?)
        return;
    
    int uiState= -1;
    if(event.type == SDL_MOUSEBUTTONDOWN)
        uiState= 1;
    else if(event.type == SDL_MOUSEBUTTONUP)
        uiState= 0;
    else
        // should never happen
        return;
    
    m_mouseX= event.x;
    m_mouseY= event.y;
    m_mouseButton= event.button;

    int uiMod= 0;
    SDLMod mod= SDL_GetModState();
    
    if ( mod & KMOD_ALT) uiMod |= ButtonFlags_Alt;
    if ( mod & KMOD_SHIFT) uiMod|= ButtonFlags_Shift;
    if ( mod & KMOD_CTRL) uiMod |= ButtonFlags_Ctrl;

    UIContext::mouse(uiButton, uiState, uiMod, event.x, event.y);
}

void SdlUIContext::processKeyboardEvent( SDL_KeyboardEvent& event )
{
    if(event.type != SDL_KEYDOWN)
        return;
    
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    if(event.keysym.unicode != 0)
    {
        //~ printf("key %c\n", (unsigned char) event.keysym.unicode);
        UIContext::keyboard((unsigned char) event.keysym.unicode, x, y);
    }
    else
    {
        unsigned char special= translateKey(event.keysym.sym);
        //~ printf("special key %d\n", special);
        if(special != 0)
            UIContext::keyboard(special, x, y);
    }
}

void SdlUIContext::processState( )
{
    // detect mouse button event from mouse state change
    int uiState= -1;
    int uiButton= -1;
    int x, y;
    
    int button= SDL_GetMouseState(&x, &y);
    if(button != m_mouseButton)
    {
        if((button & SDL_BUTTON(1)) && (m_mouseButton & SDL_BUTTON(1)) == 0)
        {
            // down event on left button
            uiButton= MouseButton_Left;
            uiState= 1;
        }
        else if((button & SDL_BUTTON(1)) == 0 && (m_mouseButton & SDL_BUTTON(1)))
        {
            // up event on left button
            uiButton= MouseButton_Left;
            uiState= 0;
        }
        else if((button & SDL_BUTTON(2)) && (m_mouseButton & SDL_BUTTON(2)) == 0)
        {
            // down event on middle button
            uiButton= MouseButton_Middle;
            uiState= 1;
        }
        else if((button & SDL_BUTTON(2)) == 0 && (m_mouseButton & SDL_BUTTON(2)))
        {
            // up event on middle button
            uiButton= MouseButton_Middle;
            uiState= 0;
        }
        else if((button & SDL_BUTTON(3)) && (m_mouseButton & SDL_BUTTON(3)) == 0)
        {
            // down event on right button
            uiButton= MouseButton_Right;
            uiState= 1;
        }
        else if((button & SDL_BUTTON(3)) == 0 && (m_mouseButton & SDL_BUTTON(3)))
        {
            // up event on right button
            uiButton= MouseButton_Right;
            uiState= 0;
        }
        
        m_mouseButton= button;
        
        if(uiState != -1 && uiButton != -1)
        {
            int uiMod= 0;
            SDLMod mod= SDL_GetModState();
            
            if ( mod & KMOD_ALT) uiMod |= ButtonFlags_Alt;
            if ( mod & KMOD_SHIFT) uiMod|= ButtonFlags_Shift;
            if ( mod & KMOD_CTRL) uiMod |= ButtonFlags_Ctrl;
            
            UIContext::mouse(uiButton, uiState, uiMod, x, y);
        }
    }
    
    // detect mouse motion events
    if(button != 0 && (m_mouseX != x || m_mouseY != y))
    {
        UIContext::mouseMotion(x, y);
        m_mouseX= x;
        m_mouseY= y;
    }
    
    // detect keyboard events 
    int nkeys;
    Uint8 *keys= SDL_GetKeyState(&nkeys);
    for(int i= 0; i < 128; i++)
    {
        if(keys[i] != 0 && m_keys[i] == 0)
            // down key event (filter out key up event)
            UIContext::keyboard(i, x, y);
        
        m_keys[i]= keys[i];
    }
    
    // detect 'special' keys
    for(int i= 128; i < nkeys; i++)
    {
        if(keys[i] != 0 && m_keys[i] == 0)
        {
            // ugly ... and slow
            unsigned char special= translateKey((SDLKey) i);
            if(special != 0)
                UIContext::keyboard(special, x, y);
            
            m_keys[i]= keys[i];
        }
    }
}


unsigned char SdlUIContext::translateKey( SDLKey k ) {
    switch (k) {
    case SDLK_F1 :
        return Key_F1;
    case SDLK_F2 :
        return Key_F2;
    case SDLK_F3 :
        return Key_F3;
    case SDLK_F4 :
        return Key_F4;
    case SDLK_F5 :
        return Key_F5;
    case SDLK_F6 :
        return Key_F6;
    case SDLK_F7 :
        return Key_F7;
    case SDLK_F8 :
        return Key_F8;
    case SDLK_F9 :
        return Key_F9;
    case SDLK_F10 :
        return Key_F10;
    case SDLK_F11 :
        return Key_F11;
    case SDLK_F12 :
        return Key_F12;
    case SDLK_LEFT :
        return Key_Left;
    case SDLK_UP :
        return Key_Up;
    case SDLK_RIGHT :
        return Key_Right;
    case SDLK_DOWN :
        return Key_Down;
    case SDLK_PAGEUP :
        return Key_PageUp;
    case SDLK_PAGEDOWN :
        return Key_PageDown;
    case SDLK_HOME :
        return Key_Home;
    case SDLK_END :
        return Key_End;
    case SDLK_INSERT :
        return Key_Insert;
    case SDLK_TAB:
        return Key_Tab;
    default:
        return 0;
    }
}

