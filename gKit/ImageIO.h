
#ifndef _IOIMAGE_H
#define _IOIMAGE_H

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "SDLImagePlatform.h"

#ifdef __cplusplus
extern "C" {
#include "rgbe.h"
}
#endif

#include "Geometry.h"
#include "IOResource.h"

#include "IOManager.h"
#include "Image.h"

namespace gk {

//! operations d'entree/sortie sur les formats standards, jpg, bmp, png, tiff, etc.
class ImageIO : public IOManager<Image>
{
    // non copyable
    ImageIO( const ImageIO& );
    ImageIO& operator=( const ImageIO& );
    
    // private default constructor, singleton
    ImageIO( )
        :
        IOManager<Image>()
    {}
        
public:
    static
    bool isColorFile( const std::string& filename )
    {
        return ((filename.rfind(".bmp") != std::string::npos)
        || (filename.rfind(".png") != std::string::npos)
        || (filename.rfind(".jpg") != std::string::npos)
        || (filename.rfind(".jpeg") != std::string::npos)
        || (filename.rfind(".tga") != std::string::npos));
    }    

    //! charge le fichier 'filename' et renvoie l'image correspondante.
    static
    Image *read( const std::string& filename, const std::string& name= "" )
    {
        // importer le fichier, si necessaire
        Image *image= manager().find(filename, name);
        if(image != NULL)
            return image;
        
    #ifdef VERBOSE
        printf("loading color image '%s'...\n", filename.c_str());
    #endif
        
        // importer le fichier
        SDL_Surface *surface= IMG_Load(filename.c_str());
        if(surface == NULL)
            return NULL;
        
        // verifier le format, rgb ou rgba
        const SDL_PixelFormat format= *surface->format;
        if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
        {
            SDL_FreeSurface(surface);
            return NULL;
        }

        // creer l'image
        const int height= surface->h;
        const int width= surface->w;
        image= new Image(width, height);
        if(image == NULL)
            return NULL;

    #ifdef VERBOSE_DEBUG
        printf("  image pixel format : "
            "bytes %d, bpp %d, "
            "red %x, shift %d, loss %d, "
            "green %x, shift %d, loss %d, "
            "blue %x, shift %d, loss %d, "
            "alpha %x, shift %d, loss %d\n",
            format.BytesPerPixel, format.BitsPerPixel,
            format.Rmask, format.Rshift, format.Rloss,
            format.Gmask, format.Gshift, format.Gloss,
            format.Bmask, format.Bshift, format.Bloss,
            format.Amask, format.Ashift, format.Aloss);
    #endif

        // converti les donnees en pixel rgba, retourne l'image, openGL utilise une origine en bas a gauche.
        Uint8 *p= (Uint8 *) surface->pixels;
        int py= 0;
        if(format.BitsPerPixel == 32)
        {
            for(int y= height -1; y >= 0; y--, py++)
            {
                p= (Uint8 *) surface->pixels + py * surface->pitch;
                for(int x= 0; x < width; x++, p+= format.BytesPerPixel)
                {
                    const Uint8 r= p[format.Rshift / 8];
                    const Uint8 g= p[format.Gshift / 8];
                    const Uint8 b= p[format.Bshift / 8];
                    const Uint8 a= p[format.Ashift / 8];
                    
                    image->setPixel(x, y, Pixel(r, g, b, a));
                }
            }
        }
        else if(format.BitsPerPixel == 24)
        {
            for(int y= height -1; y >= 0; y--, py++)
            {
                p= (Uint8 *) surface->pixels + py * surface->pitch;
                for(int x= 0; x < width; x++, p+= format.BytesPerPixel)
                {
                    const Uint8 r= p[format.Rshift / 8];
                    const Uint8 g= p[format.Gshift / 8];
                    const Uint8 b= p[format.Bshift / 8];
                    
                    image->setPixel(x, y, Pixel(r, g, b));
                }
            }
        }
    
        SDL_FreeSurface(surface);
        
        // reference l'image avec le manager
        return manager().insert(image, filename, name);
    }
    
    //! ecrit une image dans un fichier .bmp nomme 'filename'.
    static
    int write( const Image *image, const std::string& filename )
    {
        if(image == NULL)
            return -1;
        
        // flip de l'image : Y inverse entre GL et BMP
        Pixel *flip= new Pixel[image->width() * image->height()];
        Pixel *data= flip;
        for(int y= image->height() -1; y >= 0; y--)
        {
            memcpy(data, (Pixel *) image->data() + y * image->width(), image->width() * sizeof(Pixel));
            data+= image->width();
        }
        
        SDL_Surface *bmp= SDL_CreateRGBSurfaceFrom((void *) flip, 
            image->width(), image->height(), 
            32, image->width() * 4, 
    #if 0
            0xFF000000,
            0x00FF0000, 
            0x0000FF00,
            0x000000FF
    #else
            0x000000FF,
            0x0000FF00,
            0x00FF0000, 
            0xFF000000
    #endif
        );        
        
        int code= SDL_SaveBMP(bmp, filename.c_str());
        SDL_FreeSurface(bmp);
        delete [] flip;
        return code;
    }
    
    static
    ImageIO& manager( )  // singleton
    {
        static ImageIO manager;
        return manager;
    }
};


//! operations d'entree/sortie sur une image .hdr.
class HDRImageIO : public IOManager<HDRImage>
{
    HDRImageIO( const HDRImageIO& );
    HDRImageIO& operator=( const HDRImageIO& );

    // private default constructor, singleton
    HDRImageIO( )
        :
        IOManager<HDRImage>()
    {}
    
public:
    //! charge une image rgbe, .hdr
    static
    HDRImage *RGBEread( const std::string& filename, const std::string& name= "" )
    {
        FILE *in= fopen( filename.c_str(), "rb" );
        if(in == NULL)
        {
            printf("\n -- read error '%s'\n", filename.c_str());
            return NULL;
        }
        
        rgbe_header_info info;
        int width, height;
        int code = RGBE_ReadHeader(in, &width, &height, &info);
        if(code != RGBE_RETURN_SUCCESS)
        {
            fclose(in);
            printf("\n -- read error '%s'\n", filename.c_str());
            return NULL;
        }
        
        float *data= new float[width*height*3];
        code= RGBE_ReadPixels_RLE(in, data, width, height);
        if(code != RGBE_RETURN_SUCCESS)
        {
            fclose(in);
            delete [] data;
            printf("\n -- read error '%s'\n", filename.c_str());
            return NULL;
        }
        
        fclose(in);
        
        // converti les donnees en pixels hdr, retourne l'image, openGL utilise une origine en bas a gauche.
        HDRImage *image= new HDRImage(width, height);
        if(image == NULL)
            return NULL;
        
        float *p= data;
        for(int y= height -1; y >= 0; y--)
            for(int x= 0; x < width; x++, p+= 3)
                image->setPixel(x, y, HDRPixel(p[0], p[1], p[2]));
        
        delete [] data;
        
        // reference l'image avec le manager
        return manager().insert(image, filename, name);
    }
    
    static
    bool is_rgbe_file( const std::string& filename )
    {
        return (filename.rfind(".hdr") != std::string::npos);
    }
    
    //! charge une image openexr, .exr
    static
    HDRImage *EXRread( const std::string& filename, const std::string& name= "" )
    {
        return NULL;
    }

    static
    bool is_exr_file( const std::string& filename )
    {
        return (filename.rfind(".exr") != std::string::npos);
    }
    
    static
    bool isHDRFile( const std::string& filename )
    {
        return is_rgbe_file(filename);  // || is_exr_file(filename)
    }
    
    //! charge le fichier 'filename' et renvoie l'image correspondante.
    static
    HDRImage *read( const std::string& filename, const std::string& name= "" )
    {
        // importer le fichier, si necessaire
        HDRImage *image= manager().find(filename, name);
        if(image != NULL)
            return image;
        
    #ifdef VERBOSE
        printf("loading image '%s'...\n", filename.c_str());
    #endif
        
        if(is_rgbe_file(filename))
            return RGBEread(filename, name);
        
        if(ImageIO::isColorFile(filename))
        {
            // charger...
            Image *color= ImageIO::read(filename, name);
            if(color == NULL)
                return NULL;
            
            //... et convertir l'image
            HDRImage *hdr= new HDRImage(color->width(), color->height());
            
            for(int y= 0; y < color->height(); y++)
                for(int x= 0; x < color->width(); x++)
                    hdr->setPixel(x, y, 
                        color->getPixel(x, y));
            
            // reference l'image avec le manager
            return manager().insert(hdr, filename, name);
        }
        
        // format non supporte.
        return NULL;
    }

    //! enregistre l'image avec le nom 'filename'.
    static
    int write( const HDRImage *image, const std::string& filename )
    {
        if(image == NULL)
            return -1;
        
        FILE *out= fopen(filename.c_str(), "wb");
        if(out == NULL)
        {
            printf("\n -- write error '%s'\n", filename.c_str());
            return -1;
        }
        
        int code= RGBE_WriteHeader(out, image->width(), image->height(), NULL);
        if(code != RGBE_RETURN_SUCCESS)
        {
            printf("\n -- write error '%s'\n", filename.c_str());
            fclose(out);
            return -1;
        }
        
        float *data= new float[image->width() * image->height() * 3];
        float *p= data;
        for(int y= image->height() -1; y > 0; y--)
        {
            for(int x= 0; x < image->width(); x++, p+= 3)
            {
                const HDRPixel& pixel= image->getPixel(x, y);
                p[0]= pixel.r;
                p[1]= pixel.g;
                p[2]= pixel.b;
            }
        }
        
        code= RGBE_WritePixels_RLE(out, (const float *) data, image->width(), image->height());
        fclose(out);
        delete [] data;
        
        if(code != RGBE_RETURN_SUCCESS)
        {
            printf("\n -- write error '%s'\n", filename.c_str());
            return -1;
        }

        return 0;
    }
    
    static
    HDRImageIO& manager( )  // singleton
    {
        static HDRImageIO manager;
        return manager;
    }
};

}       // namespace

#endif
