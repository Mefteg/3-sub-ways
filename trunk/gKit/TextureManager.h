
#ifndef _GKTEXTURE_MANAGER_H
#define _GKTEXTURE_MANAGER_H

#include "GLManager.h"
#include "GL/GLPlatform.h"
#include "GL/TPTexture.h"


namespace gk {

//! gestion 'auto' des ressources openGL : pour les textures couleurs.
inline
GLTexture2D *createTexture2D( const int unit, const int w, const int h, const GLenum format= GL_RGBA, 
    const GLenum data_format= GL_RGBA, const GLenum data_type= GL_UNSIGNED_BYTE )
{
    return GLManager<GLTexture2D>::manager().insert( 
        new GLTexture2D(unit, w, h, format, data_format, data_type) );
}

//! gestion 'auto' des ressources openGL : pour les textures couleurs.
inline
GLTexture2D *createTexture2D( const int unit, const HDRImage *image, const GLenum format= GL_RGBA32F, 
    const GLenum data_format= GL_RGBA, const GLenum data_type= GL_FLOAT )
{
    return GLManager<GLTexture2D>::manager().insert(
        new GLTexture2D(unit, image, format, data_format, data_type) );
}

//! gestion 'auto' des ressources openGL : pour les textures couleurs.
inline
GLTexture2D *createTexture2D( const int unit, const Image *image, const GLenum format= GL_RGBA, 
    const GLenum data_format= GL_RGBA, const GLenum data_type=GL_UNSIGNED_BYTE )
{
    return GLManager<GLTexture2D>::manager().insert(
        new GLTexture2D(unit, image, format, data_format, data_type) );
}

//! gestion 'auto' des ressources openGL : pour les textures profondeur.
inline
GLDepthTexture *createDepthTexture( const int unit, const int w, const int h, const GLenum format= GL_DEPTH_COMPONENT,
    const GLenum data_format= GL_DEPTH_COMPONENT, const GLenum data_type= GL_FLOAT)
{
    return GLManager<GLDepthTexture>::manager().insert(
        new GLDepthTexture(unit, w, h, format, data_format, data_type) );
}

    
//! gestion 'auto' des ressources openGL : pour les tableaux de textures 2d.
inline
GLTexture2DArray *createTexture2DArray( const int unit, const int w, const int h, const int count, const GLenum format= GL_RGBA,
    const GLenum data_format= GL_RGBA, const GLenum data_type= GL_UNSIGNED_BYTE )
{
    return GLManager<GLTexture2DArray>::manager().insert(
        new GLTexture2DArray(unit, w, h, count, format, data_format, data_type) );
}

//! gestion 'auto' des ressources openGL : pour les tableaux de textures 2d.
inline
GLTexture2DArray *createTexture2DArray( const int unit, const HDRImageArray *images, const GLenum format= GL_RGBA32F,
    const GLenum data_format= GL_RGBA, const GLenum data_type= GL_FLOAT )
{
    return GLManager<GLTexture2DArray>::manager().insert(
        new GLTexture2DArray(unit, images, format, data_format, data_type) );
}
    
//! gestion 'auto' des ressources openGL : pour les tableaux de textures 2d.
inline
GLTexture2DArray *createTexture2DArray( const int unit, const ImageArray *images, const GLenum format= GL_RGBA,
    const GLenum data_format= GL_RGBA, const GLenum data_type=GL_UNSIGNED_BYTE )
{
    return GLManager<GLTexture2DArray>::manager().insert(
        new GLTexture2DArray(unit, images, format, data_format, data_type) );
}

//! \todo other types of textures declared in GL/TPTexture.h

}

#endif
