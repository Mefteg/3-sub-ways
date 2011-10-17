
#ifndef _GK_GLPLATFORM_H
#define _GK_GLPLATFORM_H

#ifdef APPLE_OSX
    #include <GL/glew.h>
    #include <OpenGL/gl.h>

#elif defined WIN32
    #include <GL/glew.h>
    #include <GL/gl.h>

#else
    #include "GL/glew.h"
    #include <GL/gl.h>
    //~ #define GL3_PROTOTYPES
    //~ #include "GL/gl3.h"
#endif

#if defined GK_OPENGL3 && !defined GLEW_VERSION_3_3
    #error glew does not support openGL 3.3
#endif

#if defined GK_OPENGL4 && !defined GLEW_VERSION_4_1
    #error glew does not support openGL 4.1
#endif

#endif
