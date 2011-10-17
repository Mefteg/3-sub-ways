
#include <cstdio>

#include "GL/TPTexture.h"
#include "GL/TPTextureUnits.h"

namespace gk {

GLTexture2D::GLTexture2D( const int unit, const int w, const int h, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture(GL_TEXTURE_2D)
{
    assert(glGetError() == GL_NO_ERROR);
    
    m_width= w;
    m_height= h;
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    
    if(0)
    {
        glTexImage2D(GL_PROXY_TEXTURE_2D, 0, 
            m_format, m_width, m_height, 0,
            data_format, data_type, NULL);
        
        GLint internal_format= 0;
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, 
            GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
        printf("GLTexture2D( ): texture format: requested 0x%x, optimal 0x%x\n",
            format, internal_format);
        
        GLint type, size;
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_RED_TYPE, &type);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_RED_SIZE, &size);
        printf("  red type 0x%x, size %d\n", type, size);
        
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_GREEN_TYPE, &type);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_GREEN_SIZE, &size);
        printf("  green type 0x%x, size %d\n", type, size);
        
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_BLUE_TYPE, &type);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_BLUE_SIZE, &size);
        printf("  blue type 0x%x, size %d\n", type, size);
        
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_ALPHA_TYPE, &type);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_ALPHA_SIZE, &size);
        printf("  alpha type 0x%x, size %d\n", type, size);
        
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_DEPTH_TYPE, &type);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,
            GL_TEXTURE_DEPTH_SIZE, &size);
        printf("  depth type 0x%x, size %d\n", type, size);        
    }
    
    glTexImage2D(m_target, 0, 
        m_format, m_width, m_height, 0,
        data_format, data_type, NULL);
    
    // definir les parametres de filtrages de base
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenerateMipmap(m_target);
    assert(glGetError() == GL_NO_ERROR);
}

GLTexture2D::GLTexture2D( const int unit, const HDRImage *image, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture(GL_TEXTURE_2D)
{
    if(image == NULL)
        return; // utiliser une texture debug ?? damier rouge / orange ?
    
    m_width= image->width();
    m_height= image->height();
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage2D(m_target, 0, 
        m_format, m_width, m_height, 0,
        data_format, data_type, image->data());
    
    // definir les parametres de filtrages de base
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenerateMipmap(m_target);
    assert(glGetError() == GL_NO_ERROR);    
}

GLTexture2D::GLTexture2D( const int unit, const Image *image, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture(GL_TEXTURE_2D)
{
    if(image == NULL)
        return; // utiliser une texture debug ?? damier rouge / orange ?
    
    m_width= image->width();
    m_height= image->height();
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage2D(m_target, 0, 
        m_format, m_width, m_height, 0,
        data_format, data_type, image->data());
    
    // definir les parametres de filtrages de base
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenerateMipmap(m_target);
    assert(glGetError() == GL_NO_ERROR);
}


GLDepthTexture::GLDepthTexture( const int unit, const int w, const int h, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_2D)
{
    assert(glGetError() == GL_NO_ERROR);
    
    m_width= w;
    m_height= h;
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage2D(m_target, 0, 
        m_format, m_width, m_height, 0,
        data_format, data_type, NULL);
    
    // definir les parametres de filtrages de base ...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // ... ils seront remplaces par ceux d'un sampler, normalement.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);    
    
    glGenerateMipmap(m_target);
    assert(glGetError() == GL_NO_ERROR);
}

GLTexture2DArray::GLTexture2DArray( const int unit, const int w, const int h, const int count, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_2D_ARRAY)
{
    if(m_name == 0)
        return;
    
    m_width= w;
    m_height= h;
    m_depth= count;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;

    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        m_format, m_width, m_height, m_depth, 0,
        data_format, data_type, NULL);
}

GLTexture2DArray::GLTexture2DArray( const int unit, const HDRImageArray *images, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_2D_ARRAY)
{
    if(m_name == 0)
        return;
    if(images == NULL)
        return;
    
    m_width= images->width();
    m_height= images->height();
    m_depth= images->size();
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        m_format, m_width, m_height, m_depth, 0,
        data_format, data_type, NULL);

    for(int i= 0; i < m_depth; i++)
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 
            0, 0, i,  
            m_width, m_height, 1, 
            data_format, data_type, (*images)[i]->data());
    
    glGenerateMipmap(m_target);
}

GLTexture2DArray::GLTexture2DArray( const int unit, const ImageArray *images, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_2D_ARRAY)
{
    if(m_name == 0)
        return;
    if(images == NULL)
        return;
    
    m_width= images->width();
    m_height= images->height();
    m_depth= images->size();
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        m_format, m_width, m_height, m_depth, 0,
        data_format, data_type, NULL);

    for(int i= 0; i < m_depth; i++)
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 
            0, 0, i,  
            m_width, m_height, 1, 
            data_format, data_type, (*images)[i]->data());
    
    glGenerateMipmap(m_target);
}


GLTextureCube::GLTextureCube( const int unit, const int w, const int h, const GLenum format,
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_CUBE_MAP)
{
    if(m_name == 0)
        return;
    
    m_width= w;
    m_height= h;
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    for(int face= 0; face < 6; face++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 
            0, m_format, m_width, m_height, 0,
            data_format, data_type, NULL);
    
    glGenerateMipmap(m_target);
}

GLTextureCube::GLTextureCube( const int unit, const HDRImageCube *faces, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_CUBE_MAP)
{
    if(m_name == 0)
        return;
    if(faces == NULL)
        return;
    
    m_width= faces->width();
    m_height= faces->height();
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;

    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    for(int face= 0; face < 6; face++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 
            0, m_format, m_width, m_height, 0,
            data_format, data_type, (*faces)[face]->data());
    
    glGenerateMipmap(m_target);
}

GLTextureCube::GLTextureCube( const int unit, const ImageCube *faces, const GLenum format, 
    const GLenum data_format, const GLenum data_type )
    :
    GLTexture2D(GL_TEXTURE_CUBE_MAP)
{
    if(m_name == 0)
        return;
    if(faces == NULL)
        return;
    
    m_width= faces->width();
    m_height= faces->height();
    m_depth= 1;
    m_format= format;
    m_data_format= data_format;
    m_data_type= data_type;
    
    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), this );
    for(int face= 0; face < 6; face++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 
            0, m_format, m_width, m_height, 0,
            data_format, data_type, (*faces)[face]->data());
    
    glGenerateMipmap(m_target);
}

}
