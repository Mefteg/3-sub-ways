
#ifndef _TP_PROGRAM_NAME_H
#define _TP_PROGRAM_NAME_H

#include "GL/GLPlatform.h"

namespace gk {
    
class GLShaderProgram;

//! utilisation interne. identifiant generique.
class ProgramName
{
    const GLShaderProgram *m_program;
    GLint m_location;
    GLint m_index;
    bool m_is_integer;
    
public:
    ProgramName( )
        :
        m_program(NULL),
        m_location(-1),
        m_index(-1),
        m_is_integer(false)
    {}

    explicit ProgramName( const GLint index )
        :
        m_program(NULL),
        m_location(index),
        m_index(index),
        m_is_integer(false)
    {}
    
    explicit ProgramName( const GLShaderProgram *program, const GLint location, const GLint index= -1, const bool is_integer= false ) 
        :
        m_program(program),
        m_location(location),
        m_index(index),
        m_is_integer(is_integer)
    {}
    
    ~ProgramName( ) {}
    
    const GLShaderProgram *program( ) const
    {
        return m_program;
    }
    
    const GLShaderProgram *program( )
    {
        return m_program;
    }
    
    GLint location( ) const
    {
        return m_location;
    }
    
    GLint index( ) const
    {
        return m_index;
    }
    
    bool isValid( ) const
    {
        return (m_program != NULL && m_location >= 0);
    }
    
    operator GLint ( ) const
    {
        return m_location;
    }
    
    bool isIntegerType( ) const
    {
        return m_is_integer;
    }
};


//! identifiant d'un uniform.
typedef ProgramName ProgramUniform;
//! identifiant d'un attribut.
typedef ProgramName ProgramAttribute;
//! identifiant d'un draw buffer / fragment varying.
typedef ProgramName ProgramDrawbuffer;
//! identifiant d'un transform feedback varying.
typedef ProgramName ProgramFeedback;
//! identifiant d'un sampler.
typedef ProgramName ProgramSampler;


//! modifier la valeur d'un sampler : index de l'unite de texture.
int setSamplerUniform( const ProgramSampler& sampler, const int unit );

//! modifier la valeur d'un uniform, unsigned int.
int setUniform( const ProgramUniform& uniform, const unsigned int x );
//! modifier la valeur d'un uniform, uvec2.
int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y );
//! modifier la valeur d'un uniform, uvec3.
int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y, const unsigned int z );
//! modifier la valeur d'un uniform, uvec4.
int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int w );

//! modifier la valeur d'un uniform, int.
int setUniform( const ProgramUniform& uniform, const int x );
//! modifier la valeur d'un uniform, ivec2.
int setUniform( const ProgramUniform& uniform, const int x, const int y );
//! modifier la valeur d'un uniform, ivec3.
int setUniform( const ProgramUniform& uniform, const int x, const int y, const int z );
//! modifier la valeur d'un uniform, ivec4.
int setUniform( const ProgramUniform& uniform, const int x, const int y, const int z, const int w );

//! modifier la valeur d'un uniform, float.
int setUniform( const ProgramUniform& uniform, const float x );
//! modifier la valeur d'un uniform, vec2.
int setUniform( const ProgramUniform& uniform, const float x, const float y );
//! modifier la valeur d'un uniform, vec3.
int setUniform( const ProgramUniform& uniform, const float x, const float y, const float z );
//! modifier la valeur d'un uniform, vec4.
int setUniform( const ProgramUniform& uniform, const float x, const float y, const float z, const float w );

//! modifier la valeur d'un uniform, mat4.
int setUniform( const ProgramUniform& uniform, const float *m, const GLboolean transpose= GL_TRUE );

}

#endif
