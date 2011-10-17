
#ifndef _TP_SAMPLER_H
#define _TP_SAMPLER_H

#include "GLResource.h"
#include "GL/GLPlatform.h"


namespace gk {

class GLSampler : public GLResource
{
    // non copyable
    GLSampler( const GLSampler& );
    GLSampler& operator= ( const GLSampler& );
    
protected:
    GLuint m_name;

public:
    GLSampler( )
        :
        m_name(0)
    {
        glGenSamplers(1, &m_name);
    }

    virtual ~GLSampler( )
    {
        glDeleteSamplers(1, &m_name);
    }

    int createGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    int releaseGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    GLuint name( ) const
    {
        return m_name;
    }

    int bind( const unsigned int unit )
    {
        if(m_name == 0)
            return -1;
        glBindSampler(unit, m_name);
        return 0;
    }
    
    int unbind( const unsigned int unit )
    {
        if(m_name == 0)
            return -1;
        glBindSampler(unit, 0);
        return 0;
    }
    
    int parameter( const GLenum parameter, const int value )
    {
        if(m_name == 0)
            return -1;
        glSamplerParameteri(m_name, parameter, value);
        return 0;
    }
    
    int parameter( const GLenum parameter, const float value )
    {
        if(m_name == 0)
            return -1;
        glSamplerParameterf(m_name, parameter, value);
        return 0;
    }
};

class GLNearestSampler : public GLSampler
{
public:
    GLNearestSampler( )
        :
        GLSampler()
    {
        if(m_name == 0)
            return;
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //~ glSamplerParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(m_name, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    ~GLNearestSampler( ) {}
};

class GLLinearSampler : public GLSampler
{
public:
    GLLinearSampler( )
        :
        GLSampler( )
    {
        if(m_name == 0)
            return;
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(m_name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    ~GLLinearSampler( ) {}
};

class GLDepthSampler : public GLSampler
{
public:
    GLDepthSampler( )
        :
        GLSampler()
    {
        if(m_name == 0)
            return;
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(m_name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(m_name, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glSamplerParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //~ glSamplerParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(m_name, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
};

}

#endif
