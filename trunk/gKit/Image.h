
#ifndef _IMAGE_H
#define _IMAGE_H

#include <cassert>
#include <cstdlib>
#include <string>

#include "IOResource.h"

namespace gk {

//! representation d'un pixel 'classique' rgba.
struct Pixel
{
    //! composantes publiques.
    unsigned char r, g, b, a;
    
    typedef unsigned char type;
    enum { d= 4 };
    
    static
    bool isColorPixel( )
    {
        return true;
    }
    
    static 
    bool isHdrPixel( )
    {
        return false;
    }
    
    //! par defaut, un pixel noir (opaque).
    Pixel( )
        :
        r(0), g(0), b(0), a(255)
    {
        assert(sizeof(Pixel) == sizeof(unsigned char[4]));
    }
    
    //! cree un pixel rgba.
    Pixel( const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a= 255u )
        :
        r(_r), g(_g), b(_b), a(_a)
    {
        assert(sizeof(Pixel) == sizeof(unsigned char[4]));
    }

    ~Pixel( ) {}
};

//! representation d'un pixel 'hdr' rgba.
struct HDRPixel
{
    //! composantes publiques.
    float r, g, b, a;
    
    typedef float type;
    enum { d= 4 };

    static
    bool isColorPixel( )
    {
        return false;
    }
    
    static 
    bool isHdrPixel( )
    {
        return true;
    }
    
    //! par defaut, pixel noir.
    HDRPixel( )
        :
        r(0.f) ,g(0.f), b(0.f), a(1.f)
    {
        assert(sizeof(HDRPixel) == sizeof(float[4]));
    }
    
    //! cree un pixel hdr rgba.
    HDRPixel( const float _r, const float _g, const float _b, const float _a= 1.f )
        :
        r(_r), g(_g), b(_b), a(_a)
    {
        assert(sizeof(HDRPixel) == sizeof(float[4]));
    }
    
    //! cree un pixel hdr rgba [0 .. 1] a partir d'un pixel rgba [0 .. 255].
    HDRPixel( const Pixel& color )
        :
        r((float) color.r / 255.f), 
        g((float) color.g / 255.f), 
        b((float) color.b / 255.f),
        a((float) color.a / 255.f)
    {}
    
    ~HDRPixel( ) {}
};


//! utilisation interne. representation d'une image, parametree par le type de pixel, cf. gk::Pixel et gk::HDRPixel.
//! utiliser gk::Image et gk::HDRImage.
template< class T >
class TImage : public IOResource
{
    // non copyable
    TImage( const TImage& );
    TImage& operator=( const TImage& );
    
    T *m_data;
    int m_width;
    int m_height;

public:
    //! construction d'une image de dimension width x height.
    TImage( const int width, const int height ) 
        :
        m_width(width), 
        m_height(height)
    {
        m_data= new T[width * height];
        assert(m_data != NULL);
    }

    //! destructeur.
    ~TImage( )
    {
        delete [] m_data;
    }

    bool isColorImage( ) const
    {
        return T::isColorPixel();
    }
    
    bool isHdrImage( ) const
    {
        return T::isHdrPixel();
    }    
    
    //! renvoie les donnees brutes de l'image.
    void *data( )
    {
        return m_data;
    }

    //! renvoie les donnees brutes de l'image. 
    const void *data( ) const
    {
        return m_data;
    }

    //! renvoie la largeur de l'image.
    int width( ) const
    {
        return m_width;
    }

    //! renvoie la hauteur de l'image.
    int height( ) const
    {
        return m_height;
    }

    //! remplace la couleur d'un pixel.
    void setPixel( const int x, const int y, const T& color )
    {
        assert(x >= 0 && x < m_width);
        assert(y >= 0 && y < m_height);
        
        //~ const unsigned int id= (m_height - 1 - y) * m_width + x;
        const unsigned int id= y * m_width + x;
        m_data[id]= color;
    }

    //! renvoie la couleur d'un pixel.
    const T& getPixel( const int x, const int y ) const
    {
        assert(x >= 0 && x < m_width);
        assert(y >= 0 && y < m_height);
        
        //~ const unsigned int id= (m_height - 1 - y) * m_width + x;
        const unsigned int id= y * m_width + x;
        return m_data[id];
    }
};

//! declaration d'une image hdr, pixels rgba.
typedef TImage<HDRPixel> HDRImage;

//! declaration d'une image 'classique', avec pixels rgba.
typedef TImage<Pixel> Image;

}       // namespace

#endif
