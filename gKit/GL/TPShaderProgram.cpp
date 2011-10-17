
#include <cassert>
#include <cstring>

#include "GL/TPShaderProgram.h"
#include "GL/TPAttributes.h"
#include "GL/TPTextureUnits.h"

namespace gk {

ProgramState ShaderProgram;
ProgramState ActiveShaderProgram;

ProgramState& setShaderProgram( GLShaderProgram *program )
{
    assert(program != NULL);
    
    ShaderProgram= ActiveShaderProgram.setShaderProgram(program);
    AttributeCount= program->attributeCount();
    TextureUnitCount= program->samplerCount();
    return ShaderProgram;
}

ProgramState& resetShaderProgram( )
{
    ShaderProgram= ActiveShaderProgram.reset();
    AttributeCount= 0;
    TextureUnitCount= 0;
    return ShaderProgram;
}

ProgramState& ProgramState::setShaderProgram( const GLShaderProgram *program )
{
    assert(program != NULL);
    
    if(m_program != program)
        glUseProgram(program->name());
    
    m_program= program;
    return *this;
}

ProgramState& ProgramState::reset( )
{
    glUseProgram(0);
    m_program= NULL;
    return *this;
}

bool GLShaderProgram::is_sampler( const GLenum type )
{
    switch(type)
    {
        case  GL_SAMPLER_1D:
        case  GL_SAMPLER_2D:
        case  GL_SAMPLER_3D:
        case  GL_SAMPLER_CUBE:
        case  GL_SAMPLER_1D_SHADOW:
        case  GL_SAMPLER_2D_SHADOW:
        
    #ifdef GL_ARB_gpu_shader4
        case  GL_SAMPLER_1D_ARRAY:
        case  GL_SAMPLER_2D_ARRAY:
        case  GL_SAMPLER_1D_ARRAY_SHADOW:
        case  GL_SAMPLER_2D_ARRAY_SHADOW:
        case  GL_SAMPLER_CUBE_SHADOW:
        case  GL_UNSIGNED_INT_VEC2:
        case  GL_UNSIGNED_INT_VEC3:
        case  GL_UNSIGNED_INT_VEC4:
        case  GL_INT_SAMPLER_1D:
        case  GL_INT_SAMPLER_2D:
        case  GL_INT_SAMPLER_3D:
        case  GL_INT_SAMPLER_CUBE:
        case  GL_INT_SAMPLER_1D_ARRAY:
        case  GL_INT_SAMPLER_2D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_1D:
        case  GL_UNSIGNED_INT_SAMPLER_2D:
        case  GL_UNSIGNED_INT_SAMPLER_3D:
        case  GL_UNSIGNED_INT_SAMPLER_CUBE:
        case  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    #endif
        
    #ifdef GL_EXT_gpu_shader4
        case GL_SAMPLER_1D_ARRAY_EXT:
        case GL_SAMPLER_2D_ARRAY_EXT:
        case GL_SAMPLER_BUFFER_EXT:
        case GL_SAMPLER_1D_ARRAY_SHADOW_EXT:
        case GL_SAMPLER_2D_ARRAY_SHADOW_EXT:
        case GL_SAMPLER_CUBE_SHADOW_EXT:
        case GL_UNSIGNED_INT_VEC2_EXT:
        case GL_UNSIGNED_INT_VEC3_EXT:
        case GL_UNSIGNED_INT_VEC4_EXT:
        case GL_INT_SAMPLER_1D_EXT:
        case GL_INT_SAMPLER_2D_EXT:
        case GL_INT_SAMPLER_3D_EXT:
        case GL_INT_SAMPLER_CUBE_EXT:
        case GL_INT_SAMPLER_2D_RECT_EXT:
        case GL_INT_SAMPLER_1D_ARRAY_EXT:
        case GL_INT_SAMPLER_2D_ARRAY_EXT:
        case GL_INT_SAMPLER_BUFFER_EXT:
        case GL_UNSIGNED_INT_SAMPLER_1D_EXT:
        case GL_UNSIGNED_INT_SAMPLER_2D_EXT:
        case GL_UNSIGNED_INT_SAMPLER_3D_EXT:
        case GL_UNSIGNED_INT_SAMPLER_CUBE_EXT:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT:
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT:
    #endif
    
    #ifdef GL_ARB_texture_rectangle
        case  GL_SAMPLER_2D_RECT_ARB:
        case  GL_SAMPLER_2D_RECT_SHADOW_ARB:
    #endif
            return true;

        default:
            return false;
    }
}


bool GLShaderProgram::is_integer( const GLenum type )
{
    switch(type)
    {
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3: 
        case GL_UNSIGNED_INT_VEC4:
            return true;
        
        default:
            return false;
    }
}

//! ajoute un shader object au shader program.
int GLShaderProgram::attachShader( GLShaderObject *shader )
{
    if(shader == NULL)
    {
    #ifdef VERBOSE
        printf("GLShaderProgram( ): warning, no shader object.\n");
    #endif
        return 0;
    }

    GLenum shader_type= shader->type();
    const int n= (int) m_shaders.size();
    for(int i= 0; i < n; i++)
        if(m_shaders[i]->type() == shader_type)
        {
        #ifdef VERBOSE
            printf("GLShaderProgram( ): error duplicate shader object (type 0x%x).\n",
                shader_type);
        #endif
            return -1;
        }

    m_shaders.push_back(shader);
    return 0;
}


int GLShaderProgram::createGLResource( )
{
    if(m_name == 0)
        return -1;

    return make();
}

int GLShaderProgram::releaseGLResource( )
{
    if(m_name != 0)
        glDeleteProgram(m_name);

    m_name= 0;
    m_is_linked= false;
    m_is_validated= false;
    return 0;
}

//! construit le shader program.
int GLShaderProgram::make( )
{
    if(m_name == 0)
        return -1;

    const int n= (int) m_shaders.size();
    for(int i= 0; i < n; i++)
    {
        const GLenum type= m_shaders[i]->type();
        if(m_shaders[i]->createGLResource() < 0)
        {
        #ifdef VERBOSE
            printf("GLShaderProgram( ): error compiling shader object %d (type 0x%x).\n",
                i, type);
        #endif
            return -1;
        }

    #ifdef VERBOSE_DEBUG
        printf("GLShaderProgram( ): compiling shader object %d (type 0x%x)...\n",
            i, type);
    #endif

        glAttachShader(m_name, m_shaders[i]->name());
    }

    return link();
}

ProgramUniform GLShaderProgram::uniform( const char *name ) const
{
    if(m_name == 0)
        return ProgramUniform();
    
    return ProgramUniform( this, glGetUniformLocation(m_name, name), -1 );
}

ProgramAttribute GLShaderProgram::attribute( const char *name ) const
{
    if(m_name == 0)
        return ProgramAttribute();
    
    //~ const int location= glGetAttribLocation(m_name, name);
    //~ return ProgramAttribute( this, location, location );
    const int n= (int) m_attributes.size();
    for(int i= 0; i < n ; i++)
        if(m_attributes[i].name == name)
            return ProgramAttribute( this, m_attributes[i].location, m_attributes[i].index, m_attributes[i].is_integer );
    
#ifdef VERBOSE
    printf("ProgramAttribute('%s'): not found.\n", name);
#endif
    return ProgramAttribute();
}

ProgramSampler GLShaderProgram::sampler( const char *name ) const
{
    if(m_name == 0)
        return ProgramSampler();

    const int n= (int) m_samplers.size();
    for(int i= 0; i < n; i++)
        if(m_samplers[i].name == name)
            return ProgramSampler( this, m_samplers[i].location, m_samplers[i].index );
    
    return ProgramSampler();
}

ProgramDrawbuffer GLShaderProgram::drawbuffer( const char *name ) const
{
    if(m_name == 0)
        return ProgramDrawbuffer();
    
    return ProgramDrawbuffer( this, glGetFragDataLocation(m_name, name), glGetFragDataIndex(m_name, name) );
}

ProgramFeedback GLShaderProgram::feedback( const char *name ) const
{
    if(m_name == 0)
        return ProgramFeedback();

    const int n= (int) m_feedbacks.size();
    for(int i= 0; i < n; i++)
        if(m_feedbacks[i].name == name)
            return ProgramFeedback( this, -1, m_feedbacks[i].index );
    
#ifdef VERBOSE
    printf("ProgramAttribute('%s'): not found.\n", name);
#endif
    return ProgramFeedback();
}


//! (re-)linke le shader program.
int GLShaderProgram::link( )
{
    if(m_name == 0)
        return -1;

    m_samplers.clear();
    m_attributes.clear();
    m_feedbacks.clear();
    
    GLint code;
    glLinkProgram(m_name);
    glGetProgramiv(m_name, GL_LINK_STATUS, &code);
    if(code == GL_FALSE)
    {
        GLint length= 0;
        glGetShaderiv(m_name, GL_INFO_LOG_LENGTH, &length);
        if(length == 0)
        {
            printf("GLShaderProgram( ): error linking shader program (no info log).\nfailed.\n");
            return -1;
        }
        
        // afficher les erreurs de link
        GLchar *log= new GLchar[length];
        glGetProgramInfoLog(m_name, (GLsizei) length, NULL, log);
        
        printf("GLShaderProgram( ): error linking shader program:\n%s\nfailed.\n", log);
        delete [] log;
        return -1;
    }

    // recupere les samplers et leur affecte une unite de texture.
    GLint uniform_count= 0;
    glGetProgramiv(m_name, GL_ACTIVE_UNIFORMS, &uniform_count);
    if(uniform_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(m_name, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
        char *name= new char [max_length];
        
        int unit_id= 0;
        for(int i= 0; i < uniform_count; i++)
        {
            GLint size;
            GLenum type;
            glGetActiveUniform(m_name, i, max_length, NULL, &size, &type, name);
            GLint location= glGetUniformLocation(m_name, name);
            
            if(is_sampler(type) == false)
            {
            #ifdef VERBOSE
                printf("  uniform '%s' location %d, index %d, size %d, type 0x%x\n", 
                    name, location, i, size, type);
            #endif
                continue;
            }
            
        #ifdef VERBOSE
            printf("  sampler '%s' location %d, index %d, size %d, type 0x%x\n", 
                name, location, unit_id, size, type);
        #endif
            
            m_samplers.push_back( parameter(name, location, unit_id, false) );
            unit_id++;
        }
        
        delete [] name;
    }
    
    m_sampler_count= m_samplers.size();
    m_uniform_count= uniform_count - m_sampler_count;
    
    m_attribute_count= 0;
    glGetProgramiv(m_name, GL_ACTIVE_ATTRIBUTES, &m_attribute_count);
    if(m_attribute_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(m_name, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
        char *name= new char [max_length];
        
        for(int i= 0; i < m_attribute_count; i++)
        {
            GLint size;
            GLenum type;
            glGetActiveAttrib(m_name, i, max_length, NULL, &size, &type, name);
            
            GLint location= glGetAttribLocation(m_name, name);
        #ifdef VERBOSE
            printf("  attribute '%s' location %d, index %d, size %d, type 0x%x\n", 
                name, location, i, size, type);
        #endif
            
            m_attributes.push_back( parameter(name, location, i, is_integer(type)) );
        }
        
        delete [] name;
    }

    // recupere les feedbacks
    GLint feedback_count= 0;
    glGetProgramiv(m_name, GL_TRANSFORM_FEEDBACK_VARYINGS, &feedback_count);
    if(feedback_count > 0)
    {
        GLint max_length= 0;
        glGetProgramiv(m_name, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, &max_length);
        char *name= new char [max_length];
        
        GLint buffer_mode= 0;
        glGetProgramiv(m_name, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &buffer_mode);
    #ifdef VERBOSE
        printf("  feedback mode '%s'\n", 
            (buffer_mode == GL_SEPARATE_ATTRIBS) ? "separate" :
            (buffer_mode == GL_INTERLEAVED_ATTRIBS) ? "interleaved" : "??");
    #endif
        
        int buffer_id= 0;
        for(int i= 0; i < feedback_count; i++)
        {
            GLint size;
            GLenum type;
            glGetTransformFeedbackVarying(m_name, i, max_length, NULL, &size, &type, name);
        #ifdef VERBOSE
            printf("  feedback '%s' index %d, size %d, type 0x%x, buffer %d\n", name, i, size, type, buffer_id);
        #endif
            
            m_feedbacks.push_back( parameter(name, -1, buffer_id, is_integer(type)) );
            
            // determiner dans quel buffer le feedback sera stocke
            if(buffer_mode == GL_SEPARATE_ATTRIBS)
                buffer_id++;
            else if(buffer_mode == GL_INTERLEAVED_ATTRIBS && type == GL_NONE 
            && strcmp(name, "gl_NextBuffer") == 0)      // extension arb_feedback_transform3
                buffer_id++;
        }       //! \todo calculer le buffer layout des feedbacks
        
        delete [] name;
    }
    
    m_feedback_count= m_feedbacks.size();
    
#ifdef VERBOSE
    printf("done.\n");
#endif
    m_is_linked= true;
    return 0;
}

//! valide la configuration du shader program.
int GLShaderProgram::validate( )
{
    if(m_name == 0)
        return -1;
    if(m_is_linked == false)
        return -1;

    GLint code;
    glValidateProgram(m_name);
    glGetProgramiv(m_name, GL_VALIDATE_STATUS, &code);
    if(code == GL_FALSE)
    {
        GLint length= 0;
        glGetShaderiv(m_name, GL_INFO_LOG_LENGTH, &length);
        if(length == 0)
        {
            printf("GLShaderProgram( ): error validating shader (no info log).\n");
            return -1;
        }

        GLchar *log= new GLchar[length];
        glGetProgramInfoLog(m_name, (GLsizei) length, NULL, log);

        printf("GLShaderProgram( ): error validating shader:\n%s\n", log);
        delete [] log;
        return -1;
    }

    m_is_validated= true;
    return 0;
}

}
