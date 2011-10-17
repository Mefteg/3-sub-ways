
#ifndef _GKSHADER_MANAGER_H
#define _GKSHADER_MANAGER_H

#include "GLManager.h"
#include "GL/GLShaderObject.h"
#include "GL/GLShaderObjectIO.h"
#include "GL/TPShaderProgram.h"


namespace gk {

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 2.
inline
GL2ShaderProgram *createShaderProgram2( 
        const std::string& vertex, 
        const std::string& fragment )
{
    return GLManager<GL2ShaderProgram>::manager().insert(
        new GL2ShaderProgram(vertex, fragment));
}

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 2.
inline
GL2ShaderProgram *createShaderProgram2( 
        const TextFile *vertex, 
        const TextFile *fragment )
{
    return GLManager<GL2ShaderProgram>::manager().insert(
        new GL2ShaderProgram(vertex, fragment));
}

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 2.
inline
GL2ShaderProgram *createShaderProgram( 
        const std::string& vertex, 
        const std::string& fragment )
{
    return createShaderProgram2(vertex, fragment);
}

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 2.
inline
GL2ShaderProgram *createShaderProgram( 
        const TextFile *vertex, 
        const TextFile *fragment )
{
    return createShaderProgram2(vertex, fragment);
}

#if defined GK_OPENGL3 || defined GK_OPENGL4    
//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 3.
inline
GL3ShaderProgram *createShaderProgram3( 
    const std::string& vertex, 
    const std::string& geometry,
    const std::string& fragment )
{
    return GLManager<GL3ShaderProgram>::manager().insert(
        new GL3ShaderProgram(vertex, geometry, fragment));
}

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 3.
inline
GL3ShaderProgram *createShaderProgram3( 
    const TextFile *vertex, 
    const TextFile *geometry,
    const TextFile *fragment )
{
    return GLManager<GL3ShaderProgram>::manager().insert(
        new GL3ShaderProgram(vertex, geometry, fragment));
}
#endif

#ifdef GK_OPENGL4    
//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 4.
inline
GL4ShaderProgram *createShaderProgram4( 
    const std::string& vertex, 
    const std::string& control,
    const std::string& evaluation,
    const std::string& geometry,
    const std::string& fragment )
{
    return GLManager<GL4ShaderProgram>::manager().insert(
        new GL4ShaderProgram(vertex, control, evaluation, geometry, fragment));
}

//! gestion 'auto' des ressources openGL : pour les shader programs opengl > 4.
inline
GL4ShaderProgram *createShaderProgram4( 
    const TextFile *vertex, 
    const TextFile *control,
    const TextFile *evaluation,
    const TextFile *geometry,
    const TextFile *fragment )
{
    return GLManager<GL4ShaderProgram>::manager().insert(
        new GL4ShaderProgram(vertex, control, evaluation, geometry, fragment));
}
#endif

}

#endif
