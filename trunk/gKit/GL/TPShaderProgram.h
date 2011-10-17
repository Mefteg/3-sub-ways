
#ifndef _TP_SHADER_PROGRAM_H
#define _TP_SHADER_PROGRAM_H

#include <string>
#include <vector>

#include "GLResource.h"
#include "GLManager.h"
#include "GL/GLPlatform.h"
#include "GL/GLShaderObject.h"
#include "GL/GLShaderObjectIO.h"
#include "GL/TPProgramName.h"


namespace gk {

//! representation d'un shader program openGL.
class GLShaderProgram : public GLResource
{
    struct parameter
    {
        std::string name;
        int location;
        int index;
        bool is_integer;
        
        parameter( )
            :
            name(),
            location(-1),
            index(-1),
            is_integer(false)
        {}
        
        parameter( const char *_name, const int _location, const int _index, const bool _is_integer= false )
            :
            name(_name),
            location(_location),
            index(_index),
            is_integer(_is_integer)
        {}
        
        ~parameter( ) {}
    };
    
    // non copyable
    GLShaderProgram( const GLShaderProgram& );
    GLShaderProgram& operator=( const GLShaderProgram& );
    
protected:
    std::vector<GLShaderObject *> m_shaders;
    std::vector<parameter> m_feedbacks;
    std::vector<parameter> m_attributes;
    std::vector<parameter> m_samplers;

    GLuint m_name;
    int m_attribute_count;
    int m_uniform_count;
    int m_sampler_count;
    int m_feedback_count;

    bool m_is_linked;
    bool m_is_validated;

    int make( );

public:
    static bool is_sampler( const GLenum type );
    static bool is_integer( const GLenum type );
    
    //! constructeur par defaut.
    GLShaderProgram( )
        :
        m_shaders(),
        m_feedbacks(),
        m_name(0),
        m_is_linked(false),
        m_is_validated(false)
    {
        m_name= glCreateProgram();
    }

    //! desctructeur.
    virtual ~GLShaderProgram( ) {}

    //! ajoute un shader object au shader program.
    int attachShader( GLShaderObject *shader );

    //! \todo definir attribute, varyings et samplers avant la compilation
    //! (re-)linke le shader program.
    int link( );
    //! valide la configuration du shader program.
    int validate( );

    //! construit le shader program. creation de l'objet openGL.
    int createGLResource( );

    //! detruit l'objet openGL.
    int releaseGLResource( );

    //! renvoie l'identifiant opengl de la ressource.
    GLint name( ) const
    {
        return m_name;
    }
    
    //! renvoie le nombre d'uniforms.
    int uniformCount( ) const
    {
        return m_uniform_count;
    }
    
    //! renvoie l'identifiant d'un uniform du shader program.
    ProgramUniform uniform( const char *name ) const;

    //! renvoie le nombre de samplers.
    int samplerCount( ) const
    {
        return m_sampler_count;
    }
    
    //! renvoie l'identifiant d'un sampler du shader program.
    ProgramSampler sampler( const char *name ) const;
    
    //! renvoie le nombre d'attributs.
    int attributeCount( ) const
    {
        return m_attribute_count;
    }
    
    //! renvoie l'identifiant d'un attribut du shader program.
    ProgramAttribute attribute( const char *name ) const;

    //! renvoie le nombre de drawbuffer (varying out du fragment shader).
    int drawbufferCount( ) const
    {
        //! \todo
        return -1;
    }
    
    //! renvoie le draw buffer d'un varying du fragment shader.
    ProgramDrawbuffer drawbuffer( const char *name ) const ;
    
    //! renvoie le nombre de feedbacks
    int feedbackCount( ) const
    {
        return m_feedback_count;
    }
    
    //! renvoie l'identifiant du buffer d'un varying utilise par le transform feedback.
    ProgramFeedback feedback( const char *name ) const;
};

//! shader program openGL2, compose de :
//! - vertex shader,
//! - fragment shader.
class GL2ShaderProgram : public GLShaderProgram
{
public:
    GL2ShaderProgram( ) 
        :
        GLShaderProgram()
    {}
    
    GL2ShaderProgram( 
        const TextFile *vertex,
        const TextFile *fragment )
        :
        GLShaderProgram()
    {
        GLVertexShader *vertex_shader= GLManager<GLVertexShader>::manager().insert( 
            new GLVertexShader(vertex) );
        GLFragmentShader *fragment_shader= GLManager<GLFragmentShader>::manager().insert( 
            new GLFragmentShader(fragment) );
        
        attachShader(vertex_shader);
        attachShader(fragment_shader);
    }
    
    GL2ShaderProgram( 
        const std::string& vertex_filename,
        const std::string& fragment_filename )
        :
        GLShaderProgram()
    {
        attachShader( GLVertexShaderIO::read(vertex_filename, "vertex") );
        attachShader( GLFragmentShaderIO::read(fragment_filename, "fragment") );
    }

    ~GL2ShaderProgram( ) {}
};


#if defined GK_OPENGL3 || defined GK_OPENGL4
//! shader program openGL 3, compose de :
//! - vertex shader,
//! - geometry shader,
//! - fragment shader.
//! definir GK_OPENGL3 ou GK_OPENGL4 lors de la compilation.
//! n'est utilisable que si la version de GLEW et le driver supportent GL3 ou GL4.
class GL3ShaderProgram : public GLShaderProgram
{
public:
    GL3ShaderProgram( ) 
        :
        GLShaderProgram()
    {}
    
    GL3ShaderProgram( 
        const TextFile *vertex,
        const TextFile *geometry,
        const TextFile *fragment )
        :
        GLShaderProgram()
    {
        GLVertexShader *vertex_shader= GLManager<GLVertexShader>::manager().insert( 
            new GLVertexShader(vertex) );
        GLGeometryShader *geometry_shader= GLManager<GLGeometryShader>::manager().insert( 
            new GLGeometryShader(geometry) );
        GLFragmentShader *fragment_shader= GLManager<GLFragmentShader>::manager().insert( 
            new GLFragmentShader(fragment) );
        
        attachShader(vertex_shader);
        attachShader(geometry_shader);
        attachShader(fragment_shader);
    }

    GL3ShaderProgram( 
        const std::string& vertex_filename,
        const std::string& geometry_filename,
        const std::string& fragment_filename )
        :
        GLShaderProgram()
    {
        attachShader( GLVertexShaderIO::read(vertex_filename, "vertex") );
        attachShader( GLGeometryShaderIO::read(geometry_filename, "geometry") );
        attachShader( GLFragmentShaderIO::read(fragment_filename, "fragment") );
    }

    ~GL3ShaderProgram( ) {}
};
#endif

#ifdef GK_OPENGL4
//! shader program openGL 4, compose de :
//! - vertex shader,
//! - tesselation control shader,
//! - tesselation evaluation shader,
//! - geometry shader,
//! - fragment shader.
//! definir GK_OPENGL4 lors de la compilation.
//! n'est utilisable que si la version de GLEW et le driver supportent GL4.
class GL4ShaderProgram : public GLShaderProgram
{
public:
    GL4ShaderProgram( ) 
        :
        GLShaderProgram()
    {}
    
    GL4ShaderProgram( 
        const TextFile *vertex,
        const TextFile *control,
        const TextFile *evaluation,
        const TextFile *geometry,
        const TextFile *fragment )
        :
        GLShaderProgram()
    {
        GLVertexShader *vertex_shader= GLManager<GLVertexShader>::manager().insert( 
            new GLVertexShader(vertex) );
        GLControlShader *control_shader= GLManager<GLControlShader >::manager().insert( 
            new GLControlShader (control) );
        GLEvaluationShader *evaluation_shader= GLManager<GLEvaluationShader>::manager().insert( 
            new GLEvaluationShader(evaluation) );
        GLGeometryShader *geometry_shader= GLManager<GLGeometryShader>::manager().insert( 
            new GLGeometryShader(geometry) );
        GLFragmentShader *fragment_shader= GLManager<GLFragmentShader>::manager().insert( 
            new GLFragmentShader(fragment) );
        
        attachShader(vertex_shader);
        attachShader(control_shader);
        attachShader(evaluation_shader);
        attachShader(geometry_shader);
        attachShader(fragment_shader);
    }
    
    GL4ShaderProgram( 
        const std::string& vertex_filename,
        const std::string& control_filename,
        const std::string& evaluation_filename,
        const std::string& geometry_filename,
        const std::string& fragment_filename )
        :
        GLShaderProgram()
    {
        attachShader( GLVertexShaderIO::read(vertex_filename, "vertex") );
        attachShader( GLControlShaderIO::read(control_filename, "control") );
        attachShader( GLEvaluationShaderIO::read(evaluation_filename, "evaluation") );
        attachShader( GLGeometryShaderIO::read(geometry_filename, "geometry") );
        attachShader( GLFragmentShaderIO::read(fragment_filename, "fragment") );
    }

    ~GL4ShaderProgram( ) {}
};
#endif


class ProgramState
{
    const GLShaderProgram *m_program;
    
public:
    ProgramState( )
        :
        m_program(NULL)
    {}
    
    ~ProgramState( ) {}
    
    ProgramState& operator= ( const ProgramState& b )
    {
        m_program= b.m_program;
        return *this;
    }
    
    ProgramState& setShaderProgram( const GLShaderProgram *program );

    ProgramState& reset( );
    
    const GLShaderProgram *getShaderProgram( )
    {
        return m_program;
    }
    
    const GLShaderProgram *getShaderProgram( ) const
    {
        return m_program;
    }
};

extern ProgramState ShaderProgram;
extern ProgramState ActiveShaderProgram;

ProgramState& setShaderProgram( GLShaderProgram *program );
ProgramState& resetShaderProgram( );

}

#endif
