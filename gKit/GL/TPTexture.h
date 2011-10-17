
#ifndef _TP_TEXTURE_H
#define _TP_TEXTURE_H

#include "GLResource.h"
#include "GL/GLPlatform.h"
#include "Image.h"
#include "ImageArray.h"


namespace gk {

//! representation d'une texture openGL.
class GLTexture : public GLResource
{
protected:
    GLuint m_name;
    GLuint m_target;
    GLenum m_format;
    GLenum m_data_format;
    GLenum m_data_type;
    int m_width;
    int m_height;
    int m_depth;
    
    // non copyable
    GLTexture( const GLTexture& );
    GLTexture& operator= ( const GLTexture& );
    
public:
    GLTexture( const GLenum target )
        :
        GLResource(),
        m_target(target),
        m_format(0),
        m_data_format(0),
        m_data_type(0),
        m_width(0),
        m_height(0),
        m_depth(0)
    {
        glGenTextures(1, &m_name);
    }
    
    virtual ~GLTexture( ) 
    {
        glDeleteTextures(1, &m_name);
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
    
    GLenum target( ) const
    {
        return m_target;
    }
    
    GLenum format( ) const
    {
        return m_format;
    }
    
    GLenum dataFormat( ) const
    {
        return m_data_format;
    }
    
    GLenum dataType( ) const
    {
        return m_data_type;
    }
    
    int width( ) const
    {
        return m_width;
    }
    
    int height( ) const
    {
        return m_height;
    }

    int depth( ) const
    {
        return m_depth;
    }
};

class GLTexture1D : public GLTexture
{
public:
    GLTexture1D( )
        :
        GLTexture(GL_TEXTURE_1D)
    {}
    
    ~GLTexture1D( ) {}
};

class GLTexture1DArray : public GLTexture
{
public:
    GLTexture1DArray( )
        :
        GLTexture(GL_TEXTURE_1D_ARRAY)
    {}
    
    ~GLTexture1DArray( ) {}
};

class GLTexture2D : public GLTexture
{
public:
    GLTexture2D( const GLenum target )
        :
        GLTexture(target)
    {}
    
    virtual ~GLTexture2D( ) {}

    GLTexture2D( const int unit, const int w, const int h, const GLenum format= GL_RGBA, 
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_UNSIGNED_BYTE );
        
    GLTexture2D( const int unit, const HDRImage *image, const GLenum format= GL_RGBA32F, 
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_FLOAT );
    
    GLTexture2D( const int unit, const Image *image, const GLenum format= GL_RGBA, 
        const GLenum data_format= GL_RGBA, const GLenum data_type=GL_UNSIGNED_BYTE );
};

class GLDepthTexture : public GLTexture2D
{
public:
    GLDepthTexture( )
        :
        GLTexture2D(GL_TEXTURE_2D)
    {}
    
    GLDepthTexture( const int unit, const int w, const int h, const GLenum format= GL_DEPTH_COMPONENT,
        const GLenum data_format= GL_DEPTH_COMPONENT, const GLenum data_type= GL_FLOAT);
    
    ~GLDepthTexture( ) {}    
};

class GLTexture2DArray : public GLTexture2D
{
public:
    GLTexture2DArray( )
        :
        GLTexture2D(GL_TEXTURE_2D_ARRAY)
    {}
    
    ~GLTexture2DArray( ) {}
    
    GLTexture2DArray( const int unit, const int w, const int h, const int count, const GLenum format= GL_RGBA,
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_UNSIGNED_BYTE );
        
    GLTexture2DArray( const int unit, const HDRImageArray *images, const GLenum format= GL_RGBA32F,
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_FLOAT );
    
    GLTexture2DArray( const int unit, const ImageArray *images, const GLenum format= GL_RGBA,
        const GLenum data_format= GL_RGBA, const GLenum data_type=GL_UNSIGNED_BYTE );
};

class GLTextureCube : public GLTexture2D
{
public:
    GLTextureCube( )
        :
        GLTexture2D(GL_TEXTURE_CUBE_MAP)
    {}
    
    ~GLTextureCube( ) {}
    
    GLTextureCube( const int unit, const int w, const int h, const GLenum format= GL_RGBA,
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_UNSIGNED_BYTE );
        
    GLTextureCube( const int unit, const HDRImageCube *faces, const GLenum format= GL_RGBA32F,
        const GLenum data_format= GL_RGBA, const GLenum data_type= GL_FLOAT );
    
    GLTextureCube( const int unit, const ImageCube *faces, const GLenum format= GL_RGBA,
        const GLenum data_format= GL_RGBA, const GLenum data_type=GL_UNSIGNED_BYTE );
};

class GLTextureCubeArray : public GLTexture
{
public:
    GLTextureCubeArray( )
        :
        GLTexture(GL_TEXTURE_CUBE_MAP_ARRAY_ARB)
    {
        // verifier le support de gl_arb_texture_cube_map_array
    }
    
    ~GLTextureCubeArray( ) {}
};


class GLTexture3D : public GLTexture
{
public:
    GLTexture3D( )
        :
        GLTexture(GL_TEXTURE_3D)
    {}
    
    ~GLTexture3D( ) {}
};

}

#endif
