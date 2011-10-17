
#ifndef _GKFRAMEBUFFER_MANAGER_H
#define _GKFRAMEBUFFER_MANAGER_H

#include "GLResource.h"
#include "GL/GLPlatform.h"
#include "GL/TPFramebuffer.h"


namespace gk {

//! gestion 'auto' des rendertargets.
inline
GLRendertarget *createRendertarget( )
{
    return GLManager<GLRendertarget>::manager().insert(
        new GLRendertarget() );
}

//! gestion 'auto' des rendertargets.
inline
GLRendertarget *createRendertarget( const unsigned int state, const int w, const int h, const unsigned int buffer_bits )
{
    return GLManager<GLRendertarget>::manager().insert(
        new GLRendertarget(state, w, h, buffer_bits) );
}

//! gestion 'auto' des (draw) framebuffers.
inline
GLFramebuffer *createFramebuffer( )
{
    return GLManager<GLFramebuffer>::manager().insert(
        new GLFramebuffer() );
}

//! gestion 'auto' des (draw) framebuffers.
inline
GLFramebuffer *createFramebuffer( const int w, const int h, const unsigned int buffer_bits )
{
    return GLManager<GLFramebuffer>::manager().insert(
        new GLFramebuffer(w, h, buffer_bits) );
}

//! gestion 'auto' des (read) framebuffers.
inline
GLFramebuffer *createReadFramebuffer( )
{
    return GLManager<GLFramebuffer>::manager().insert(
        new GLFramebuffer() );
}

//! gestion 'auto' des (read) framebuffers.
inline
GLReadFramebuffer *createReadFramebuffer( const int w, const int h, const unsigned int buffer_bits )
{
    return GLManager<GLReadFramebuffer>::manager().insert(
        new GLReadFramebuffer(w, h, buffer_bits) );
}

}

#endif
