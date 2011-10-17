
#ifndef _GKEFFECT_SHADER_MANAGER_H
#define _GKEFFECT_SHADER_MANAGER_H

#include <string>
#include "Effect.h"
#include "ShaderManager.h"


namespace gk {

//! creation simplifiee d'un shader program a partir d'un script.
class EffectShaderManager 
{
    Effect *m_effect;
    
    // non copyable.
    EffectShaderManager( );
    EffectShaderManager( const EffectShaderManager& );
    EffectShaderManager& operator= ( const EffectShaderManager& );
    
public:
    //! constructeur. charge le script.
    EffectShaderManager( const std::string& effect );

    //! constructeur. reference un script deja charge.
    EffectShaderManager( Effect *effect );
    
    //! destructeur.
    ~EffectShaderManager( );

    //! cree un shader program opengl > 2 a partir d'une section complete.
    GL2ShaderProgram *createShaderProgram( const std::string& program_key )
    {
        return createShaderProgram2(program_key);
    }
    //! cree un shader program opengl > 2 a partir des sections de chaque shader.
    GL2ShaderProgram *createShaderProgram( 
        const TextValue& vertex_key, 
        const TextValue& fragment_key )
    {
        return createShaderProgram2(vertex_key, fragment_key);
    }
    
    //! cree un shader program opengl > 2 a partir d'une section complete.
    GL2ShaderProgram *createShaderProgram2( const std::string& program_key );
    
    //! cree un shader program opengl > 2 a partir des sections de chaque shader.
    GL2ShaderProgram *createShaderProgram2( 
        const TextValue& vertex_key, 
        const TextValue& fragment_key );
    
#if defined GK_OPENGL3 || defined GK_OPENGL4    
    //! cree un shader program opengl > 3 a partir d'une section complete.
    GL3ShaderProgram *createShaderProgram3( const std::string& program_key );
    //! cree un shader program opengl > 3 a partir des sections de chaque shader.
    GL3ShaderProgram *createShaderProgram3( 
        const TextValue& vertex_key,
        const TextValue& geometry_key,
        const TextValue& fragment_key );
#endif
    
#ifdef GK_OPENGL4    
    //! cree un shader program opengl > 4 a partir d'une section complete.
    GL4ShaderProgram *createShaderProgram4( const std::string& program_key );
    //! cree un shader program opengl > 4 a partir des sections de chaque shader.
    GL4ShaderProgram *createShaderProgram4( 
        const TextValue& vertex_key,
        const TextValue& control_key,
        const TextValue& evaluation_key,
        const TextValue& goemetry_key,
        const TextValue& fragment_key );
#endif
};

}

#endif
