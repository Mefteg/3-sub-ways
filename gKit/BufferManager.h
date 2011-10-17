
#ifndef _GKBUFFER_MANAGER_H
#define _GKBUFFER_MANAGER_H

#include "GLManager.h"
#include "GL/GLPlatform.h"
#include "GL/TPBuffer.h"


namespace gk {

//! gestion 'auto' des ressources openGL : pour les buffers.
inline
GLAttributeBuffer *createAttributeBuffer( const unsigned int count, const unsigned int size, const void *data, const GLenum usage= GL_STATIC_DRAW )
{
    return GLManager<GLAttributeBuffer>::manager().insert(
        new GLAttributeBuffer(count, size, data, usage) );
}

//! gestion 'auto' des ressources openGL : pour les buffers.
inline
GLIndexBuffer *createIndexBuffer( const unsigned int count, const unsigned int size, const void *data, const GLenum usage= GL_STATIC_DRAW )
{
    return GLManager<GLIndexBuffer>::manager().insert(
        new GLIndexBuffer(count, size, data, usage) );
}

}

#endif
