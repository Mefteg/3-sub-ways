
#ifndef _GK_SDLPLATFORM_H
#define _GK_SDLPLATFORM_H

#ifdef APPLE_OSX
    #include <SDL/SDL.h>
    
#elif defined WIN32
    #include <SDL.h>
    
#else
    #include <SDL/SDL.h>
#endif

#endif
