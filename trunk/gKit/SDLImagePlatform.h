
#ifndef _GK_SDLIMAGEPLATFORM_H
#define _GK_SDLIMAGEPLATFORM_H

#ifdef APPLE_OSX
    #include <SDL_image/SDL_image.h>

#elif defined WIN32
    #include <SDL_image.h>

#else
    #include <SDL/SDL_image.h>
#endif

#endif
