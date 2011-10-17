
#include <cassert>
#include <cstdio>

#include "GL/GLPlatform.h"

#include "App.h"
#include "ProfilerClock.h"

namespace gk {

App::App( )
    :
    m_key_state(NULL), m_key_map(NULL),
    m_width(0), m_height(0),
    m_stop(0)
{
    if(createWindow(1024, 768) < 0)
        Close();
}

App::App( const int w, const int h )
    :
    m_key_state(NULL), m_key_map(NULL),
    m_width(0), m_height(0),
    m_stop(0)
{
    if(createWindow(w, h) < 0)
        Close();
}

App::~App( )
{
    delete [] m_key_state;
    delete [] m_key_map;
}


int App::resizeWindow( const int w, const int h, const unsigned int flags )
{
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    
    // redimensionnement la fenetre
    int width= (w != 0) ? w : m_width;
    int height= (h != 0) ? h : m_height;
    unsigned int mode_flags= SDL_OPENGL | flags;
    if(flags == 0)
        mode_flags|= SDL_RESIZABLE;
    
    if((flags & SDL_FULLSCREEN) != 0)
    {
        SDL_Rect **modes= SDL_ListModes(NULL, SDL_OPENGL | SDL_FULLSCREEN);
        if(modes != (SDL_Rect **) -1)
        {
            width= modes[0]->w;
            height= modes[0]->h;
            printf("App::resizeWindow( ): no frame / fullscreen %d x %d\n", width, height);
        }
    }
    
    if(SDL_GetVideoInfo() == NULL 
    || SDL_SetVideoMode(width, height, 0, mode_flags) < 0)
    {
        printf("SDL_SetVideoMode() failed:\n%s\n", SDL_GetError());
        return -1;
    }
    
    // conserve la nouvelle taille
    m_width= width;
    m_height= height;
    return 0;
}

int App::createWindow( const int w, const int h )
{
    // initialise SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0 
    || SDL_GetVideoInfo() == NULL )
    {
        printf("SDL_Init() failed:\n%s\n", SDL_GetError());
        return -1;
    }
    
    // enregistre le destructeur de sdl
    atexit(SDL_Quit);
    
    // fixe le titre de la fenetre
    SDL_WM_SetCaption("gKit", "");
    
    // clavier unicode
    SDL_EnableUNICODE(1);
    
    // + contournement d'un bug de sdl windows sur claviers non qwerty
    int keys_n= 0;
    SDL_GetKeyState(&keys_n);
    //~ SDL_GetKeyboardState(&keys_n);  // sdl 1.3
    
    m_key_state= new unsigned char[keys_n];
    m_key_map= new unsigned int[keys_n];
    for(int i= 0; i < keys_n; i++)
    {
        m_key_state[i]= 0;
        m_key_map[i]= i;
    }
    
    // creer la fenetre et le contexte openGL
    if(resizeWindow(w, h) < 0)
        return -1;
    
#if 1
    {
        printf("openGL version: '%s'\nGLSL version: '%s'\n",
            glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
#endif
    
    {
        int swap= 0;
        SDL_GL_GetAttribute(SDL_GL_SWAP_CONTROL, &swap);
        printf("swap control: %s\n", swap ? "on" : "OFF");
    }
    
    {
        int direct_rendering= 0;
        SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &direct_rendering);
        printf("direct rendering: %s\n", direct_rendering ? "on" : "OFF");
    }
    
    // initialise les extensions openGL 
    GLenum err;
    err= glewInit();
    if(err != GLEW_OK)
    {
        printf("%s\n", glewGetErrorString(err));
        return -1;
    }

    #ifdef GK_OPENGL3
    if(!GLEW_VERSION_3_3)
    {
        printf("openGL 3.3 not supported.\n");
        return -1;
    }
    #endif

    #ifdef GK_OPENGL4
    if(!GLEW_VERSION_4_0)
    {
        printf("openGL 4.0 not supported.\n");
        return -1;
    }
    if(!GLEW_VERSION_4_1)
    {
        printf("openGL 4.1 not supported.\n");
    }
    #endif
    
    if(!GLEW_VERSION_2_1)
    {
        printf("OpenGL 2: context creation failed.\n");
        return -1;
    }

    if(!glewIsSupported("GL_ARB_vertex_buffer_object"))
    {
        printf("ARB vertex buffer object: extension missing.\n");
        return -1;
    }

    if(!glewIsSupported("GL_EXT_framebuffer_object"))
    {
        printf("EXT framebuffer object: extension missing.\n");
        return -1;
    }
    
    if(!glewIsSupported("GL_ARB_framebuffer_object"))
    {
        printf("ARB framebuffer object: extension missing.\n");
    }
    
    if(!glewIsSupported("GL_ARB_texture_float"))
    {
        printf("ARB texture float: extension missing.\n");
        return -1;      // support des textures hdr RGBA32F_ARB
    }
    
    if(!glewIsSupported("GL_ARB_color_buffer_float"))
    {
        printf("ARB color buffer float: extension missing.\n");
        //~ return -1;
    }
    
    if(!glewIsSupported("GL_ARB_depth_buffer_float"))
    {
        printf("ARB depth buffer float: extension missing.\n");
        //~ return -1;
    }
    
    if(!glewIsSupported("GL_EXT_texture_filter_anisotropic"))
    {
        printf("EXT texture filter anisotropic: extension missing.\n");
        //~ return -1;
    }
    else
    {
        GLint max_aniso;
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
        printf("EXT texture filter anisotropic: max %d\n", max_aniso);
    }
    
    // fixe l'etat openGL par defaut 
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.f);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    //~ glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);    
    
    return 0;
}

bool App::isClosed( )
{
    return (m_stop == 1);
}

void App::Close( )
{
    m_stop= 1;
}

unsigned char *App::getKeys( )
{
    return m_key_state;
}

unsigned char& App::key( const int key )
{
    return m_key_state[key];

}

bool App::processEvents( )
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_VIDEORESIZE:
                resizeWindow(event.resize.w, event.resize.h);
                
                // prevenir l'application
                processWindowResize(event.resize);
                break;
                
            case SDL_QUIT:
                m_stop= 1;
                break;
                
            case SDL_KEYUP:
                // gestion clavier unicode
                // desactive la touche
                m_key_state[m_key_map[event.key.keysym.sym]]= 0;
                
                // prevenir l'application
                processKeyboardEvent(event.key);
                break;
                
            case SDL_KEYDOWN:
                // gestion clavier unicode
                m_key_state[event.key.keysym.sym]= 1;
                if(event.key.keysym.unicode < 128 && event.key.keysym.unicode > 0 )
                {
                    // conserve l'association entre le code de la touche et l'unicode 
                    m_key_map[event.key.keysym.sym]= (unsigned int) event.key.keysym.unicode;
                    // annule l'etat associe au code la touche
                    m_key_state[event.key.keysym.sym]= 0;
                    // active l'etat associe a l'unicode
                    m_key_state[event.key.keysym.unicode]= 1;
                }
                
                // prevenir l'application
                processKeyboardEvent(event.key);
                break;
            
            case SDL_MOUSEMOTION:
                // prevenir l'application
                processMouseMotionEvent(event.motion);
                break;
            
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                // prevenir l'application
                processMouseButtonEvent(event.button);
                break;
        }
    }
    
    return (m_stop == 0);
}


int App::run( )
{
    if(isClosed())
        return -1;
    
    // termine l'initialisation des classes derivees, chargement de donnees, etc.
    if(init() < 0)
    {
    #ifdef VERBOSE
        printf("App::init( ): failed.\n");
    #endif
        return -1;
    }

    ProfilerClock::Ticks start= ProfilerClock::getTicks();
    ProfilerClock::Ticks last_frame= start;
    while(!isClosed())
    {
        // traitement des evenements : clavier, souris, fenetre, etc.
        processEvents();
        
        // mise a jour de la scene
        ProfilerClock::Ticks frame= ProfilerClock::getDelay(start);
        ProfilerClock::Ticks delta= ProfilerClock::delay(frame, last_frame);
        
        if(update( frame / 1000, delta / 1000 ) == 0)
            break;
        
        // affiche la scene
        if(draw() == 0)
            break;
        
        last_frame= frame;
    }
    
    // destruction des ressources chargees par les classes derivees.
    if(quit() < 0)
        return -1;

    return 0;
}

}
