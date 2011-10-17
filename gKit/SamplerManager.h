
#ifndef _GKSAMPLER_MANAGER_H
#define _GKSAMPLER_MANAGER_H

#include "GLManager.h"
#include "GL/GLPlatform.h"
#include "GL/TPSampler.h"


namespace gk {

//! gestion 'auto' des ressources openGL : pour les samplers de textures (mipmap linear).
inline
GLSampler *createSampler( )
{
    return GLManager<GLSampler>::manager().insert( 
        new GLLinearSampler() );
}

//! gestion 'auto' des ressources openGL : pour les samplers de textures (mipmap linear).
inline
GLSampler *createLinearSampler( )
{
    return GLManager<GLSampler>::manager().insert( 
        new GLLinearSampler() );
}

//! gestion 'auto' des ressources openGL : pour les samplers de textures (nearest).
inline
GLSampler *createNearestSampler( )
{
    return GLManager<GLSampler>::manager().insert( 
        new GLNearestSampler() );
}

//! gestion 'auto' des ressources openGL : pour les samplers de textures depth component.
inline
GLSampler *createDepthSampler( )
{
    return GLManager<GLSampler>::manager().insert( 
        new GLDepthSampler() );
}

}

#endif
