
#ifndef _IMAGE_ARRAY_H
#define _IMAGE_ARRAY_H

#include <vector>

#include "Image.h"


namespace gk {

template< class T >
class TImageArray
{
    TImageArray( );
    TImageArray( const TImageArray& );
    TImageArray& operator= ( const TImageArray& );
    
protected:
    std::vector< TImage<T> *> m_images;
    int m_width;
    int m_height;
    
public:
    TImageArray( const int w, const int h, const int count= 1 )
        :
        m_images(),
        m_width(w),
        m_height(h)
    {
        m_images.resize(count);
    }

    virtual ~TImageArray( ) {}

    int width( ) const
    {
        return m_width;
    }
    
    int height( ) const
    {
        return m_height;
    }
    
    unsigned int size( ) const
    {
        return m_images.size();
    }
    
    void clear( )
    {
        m_images.clear();
    }
    
    int push_back( TImage<T> *image )
    {
        if(image == NULL)
            return -1;
        if(image->width() != m_width || image->height() != m_height)
            return -1;
        
        m_images.push_back(image);
        return 0;
    }
    
    int push_back( TImage<T>& image )
    {
        if(image.width() != m_width || image.height() != m_height)
            return -1;
        
        m_images.push_back(&image);
        return 0;
    }
    
    const TImage<T> *operator[] ( const int index ) const
    {
        if(index < 0 || index >= (int) m_images.size())
            return NULL;
        
        return m_images[index];
    }
    
    TImage<T> *operator[] ( const int index )
    {
        if(index < 0 || index >= (int) m_images.size())
            return NULL;
        
        return m_images[index];
    }
};

//! declaration d'un image array hdr, pixels rgba.
typedef TImageArray<HDRPixel> HDRImageArray;
//! declaration d'un image array, pixels rgba.
typedef TImageArray<Pixel> ImageArray;

template< class T >
class TImageCube : public TImageArray<T>
{
public:
    TImageCube( const int w, const int h )
        :
        TImageArray<T>(w, h, 6)
    {}
    
    ~TImageCube( ) {}
    
    const TImage<T> *operator[] ( const unsigned int face ) const
    {
        if(face < GL_TEXTURE_CUBE_MAP_POSITIVE_X)       //dependence opengl
            return TImageArray<T>::m_images[face];
        if(face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            return TImageArray<T>::m_images[face - GL_TEXTURE_CUBE_MAP_POSITIVE_X];
        return NULL;
    }
    
    TImage<T> *operator[] ( const unsigned int face )
    {
        if(face < GL_TEXTURE_CUBE_MAP_POSITIVE_X)       //dependence opengl
            return TImageArray<T>::m_images[face];
        if(face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
            return TImageArray<T>::m_images[face - GL_TEXTURE_CUBE_MAP_POSITIVE_X];
        return NULL;
    }
};

//! declaration d'une image cube hdr, pixels rgba.
typedef TImageCube<HDRPixel> HDRImageCube;
//! declaration d'une image cube, pixels rgba.
typedef TImageCube<Pixel> ImageCube;

}

#endif
