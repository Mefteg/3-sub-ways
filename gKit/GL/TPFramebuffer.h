
#ifndef _TP_FRAMEBUFFER_H
#define _TP_FRAMEBUFFER_H

#include "GLResource.h"
#include "GL/GLPlatform.h"
#include "GL/TPTexture.h"


namespace  gk {

class GLRendertarget;

//! utilisation interne. representation de l'etat du pipeline : framebuffer actif.
class FramebufferState
{
    GLRendertarget *m_framebuffer;
    GLenum m_target;
    int m_index;
    
    FramebufferState( const int index, const GLenum target )
        :
        m_framebuffer(NULL),
        m_target(target),
        m_index(index)
    {}
    
public:
    //! constructeur.
    FramebufferState( )
        :
        m_framebuffer(NULL),
        m_target(GL_NONE),
        m_index(-1)
    {}
    
    //! destructeur.
    ~FramebufferState( ) {}
    
    //! affectation.
    FramebufferState& operator= ( const FramebufferState& b );

    //! active un framebuffer.
    FramebufferState& setFramebuffer( GLRendertarget *framebuffer );
    //! desactive l'utilisation d'un framebuffer et repasse au framebuffer par defaut associe a la fenetre.
    FramebufferState& reset( );
    
    //! renvoie le framebuffer actif, renvoie NULL si le framebuffer actif est le framebuffer par defaut (associe a la fenetre).
    GLRendertarget *getFramebuffer( )
    {
        return m_framebuffer;
    }
    
    //! renvoie le framebuffer actif, renvoie NULL si le framebuffer actif est le framebuffer par defaut (associe a la fenetre).
    const GLRendertarget *getFramebuffer( ) const
    {
        return m_framebuffer;
    }
    
    //! renvoie le type de framebuffer associe a l'etat.
    GLenum target( ) const
    {
        assert(m_index != -1);
        return m_target;
    }
    
    //! initilisation du suivi des etats du framebuffer. appel necessaire avant toute utilisation.
    static int init( );
};

enum
{
    READ= 0,
    DRAW= 1
};

extern FramebufferState Framebuffers[2];
extern FramebufferState ActiveFramebuffers[2];

//! active un framebuffer.
FramebufferState& setFramebuffer( GLRendertarget *framebuffer );
//! desactive le framebuffer courant, repasse dans l'etat par defaut.
FramebufferState& resetFramebuffer( );

//! active un framebuffer.
FramebufferState& setReadFramebuffer( GLRendertarget *framebuffer );
//! desactive le framebuffer courant, repasse dans l'etat par defaut.
FramebufferState& resetReadFramebuffer( );

//! copie le read framebuffer [sx_min, sy_min] x (sx_max, sy_max) sur le draw framebuffer [dx_min, dy_min] x (dx_max, dy_max).
int BlitFramebuffer( const int sx_min, const int sy_min, const int sx_max, const int sy_max,
    const int dx_min, const int dy_min, const int dx_max, const int dy_max, 
    const GLbitfield buffer_bits= GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, const GLenum filter= GL_NEAREST );

//! copie le read framebuffer [sx_min, sy_min] x (sx_min + width, sy_min + height) sur le draw framebuffer [dx_min, dy_min] x (dx_min + width, dy_min + height)
inline
int BlitFramebuffer( const int sx_min, const int sy_min, const int width, const int height,
    const int dx_min, const int dy_min, 
    const GLbitfield buffer_bits= GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, const GLenum filter= GL_NEAREST )
{
    return BlitFramebuffer(
        sx_min, sy_min, sx_min + width, sy_min + height,
        dx_min, dy_min, dx_min + width, dy_min + height, buffer_bits, filter );
}

//! identification des textures associees au framebuffer.
enum 
{
    COLOR0= 0,
    COLOR1= 1,
    COLOR2= 2,
    COLOR3= 3,
    COLOR4= 4,
    COLOR5= 5,
    COLOR6= 6,
    COLOR7= 7,
    LAST_COLOR= 8,
    DEPTH= 9,
    LAST= 10
};

//! creation des textures par le framebuffer : identification des textures a initialiser.
enum
{
    COLOR0_BIT= 1<<COLOR0,
    COLOR1_BIT= 1<<COLOR1,
    COLOR2_BIT= 1<<COLOR2,
    COLOR3_BIT= 1<<COLOR3,
    COLOR4_BIT= 1<<COLOR4,
    COLOR5_BIT= 1<<COLOR5,
    COLOR6_BIT= 1<<COLOR6,
    COLOR7_BIT= 1<<COLOR7,
    DEPTH_BIT= 1<<DEPTH
};

//! identification des drawbuffers.
enum
{
    DRAW0= 0,
    DRAW1= 1,
    DRAW2= 2,
    DRAW3= 3,
    DRAW4= 4,
    DRAW5= 5,
    DRAW6= 6,
    DRAW7= 7,
};

    
//! utilisation interne. representation d'un framebuffer.
class GLRendertarget : public GLResource
{
protected:
    std::vector<GLTexture *> m_textures;
    std::vector<GLenum> m_draw_buffers;
    int m_draw_buffers_count;
    int m_width;
    int m_height;
    unsigned int m_name;
    
    // non copyable
    GLRendertarget( const GLRendertarget& );
    GLRendertarget& operator= ( const GLRendertarget& );

public:
    //! constructeur par defaut.
    GLRendertarget( );

    //! associe une texture au framebuffer. buffer = gk::COLOR0 ou gk::DEPTH, etc. 
    //! toutes les textures doivent avoir les memes dimensions.\n
    /*! attention: la texture n'est pas automatiquement associee a un draw buffer, utiliser 
        - setBuffer(), 
        - setFaceBuffer(), 
        - setLayerBuffer() 
        selon le type de texture pour associer une sortie du fragment shader a un draw buffer (et a la texture correspondante).
    */
    /*! exemple d'utilisation :
    \code
    gk::GLFramebuffer framebuffer;            // cree un framebuffer "vide"
    framebuffer.attachTexture(gk::COLOR0, texture1);    // associe une texture couleur au framebuffer
    framebuffer.setDrawbuffer(gk::DRAW0, gk::COLOR0);           // associe la texture au draw buffer 0
    framebuffer.attachTexture(gk::DEPTH, texture2);     // associe une texture au z buffer.
    
    gk::setFramebuffer(&framebuffer);                                 // active le framebuffer et les textures associees.
    \endcode
    */
    int attachTexture( const unsigned int state, const unsigned int buffer, GLTexture *texture );

    //! constructeur : cree les textures et les associe au framebuffer. buffer_bits= gk::COLOR0_BIT | gk::DEPTH_BIT, par exemple.
    /*! exemple d'utilisation :
    \code
    gk::GLFramebuffer framebuffer(512, 512, gk::COLOR0_BIT | gk::DEPTH_BIT);    // cree un framebuffer + une texture couleur + une texture profondeur
    
    gk::setFramebuffer(&framebuffer); // active le framebuffer et les textures associees.
    
    gk::GLTexture *color= framebuffer.texture(gk::COLOR0);  // pour recuperer la texture associee au framebuffer.
    \endcode
    */
    GLRendertarget( const unsigned int state, const int w, const int h, const unsigned int buffer_bits );
    
    //! destructeur.
    virtual ~GLRendertarget(  );

    //! creation de l'objet opengl.
    int createGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    //! destruction de l'objet opengl.
    int releaseGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    //! verifie la configuration du framebuffer.
    int validate( const unsigned int state );
    
    //! active le framebuffer.
    int bind( const unsigned int state );
    //! desactive le framebuffer.
    int unbind( const unsigned int state );
    
    //! renvoie le nom de l'objet opengl.
    GLuint name( ) const
    {
        return m_name;
    }
    
    //! renvoie la largeur du framebuffer. renvoie 0, si la largeur du framebuffer n'est pas definie.
    int width( ) const
    {
        return m_width;
    }
    
    //! renvoie la hauteur du framebuffer. renvoie 0, si la hauteur du framebuffer n'est pas definie.
    int height( ) const
    {
        return m_height;
    }
    
    //! renvoie la texture associee a buffer (cf gk::COLOR0, gk::COLOR1, gk::DEPTH, etc.)
    GLTexture *texture( const unsigned int buffer );
    
    //! associe une texture du framebuffer ('buffer', gk::COLOR0) a un drawbuffer ('draw_buffer', gk::DRAW0). texture2d classique.
    /*! exemple d'utilisation :
    \code
    gk::GLFramebuffer framebuffer;
    framebuffer.attachTexture(gk::COLOR0, texture1);
    framebuffer.attachTexture(gk::COLOR2, texture2);
    framebuffer.attachTexture(gk::DEPTH, textured);
    
    framebuffer.setDrawbuffer(gk::DRAW0, gk::COLOR0);
    framebuffer.setDrawbuffer(gk::DRAW1, gk::COLOR2);
    framebuffer.setDrawbuffer(gk::DEPTH, gk::DEPTH);
    
    gk::setFramebuffer(&framebuffer);
    \endcode
    */
    
    /*! exemple avec un shader :
    \code
    gk::GLShaderProgram *program= ...;
    
    gk::GLFramebuffer framebuffer;
    framebuffer.attachTexture(gk::COLOR0, texture1);
    framebuffer.attachTexture(gk::COLOR2, texture2);
    framebuffer.attachTexture(gk::DEPTH, textured);
    
    framebuffer.setDrawbuffer(program->drawbuffer("color"), gk::COLOR0);
    framebuffer.setDrawbuffer(program->drawbuffer("normal"), gk::COLOR2);
    framebuffer.setDrawbuffer(gk::DEPTH, gk::DEPTH);
    
    gk::setFramebuffer(&framebuffer);
    \endcode
     */
    int setBuffer( const unsigned int state, const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 );
    //! associe une face d'une texture cube au framebuffer. cf. GLTextureCube. 
    int setFaceBuffer( const unsigned int state, const GLenum face, const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 );
    //! associe un layer d'un texture2DArray au framebuffer. cf. GLTexture2DArray.
    int setLayerBuffer( const unsigned int state, const unsigned int layer, const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 );
    
    //! desactive un drawbuffer du framebuffer.
    int resetBuffer( const unsigned int state, const unsigned int draw_buffer );
    
    //! met a jour les mipmaps de la texture attachee au framebuffer apres un rendu.
    int updateBufferMipmap( const unsigned int buffer, const unsigned int unit= 0 );
};

//! representation d'un (draw) framebuffer.
class GLFramebuffer : public GLRendertarget
{
public:
    //! constructeur.
    GLFramebuffer(  )
        :
        GLRendertarget( )
    {}
    
    //! constructeur, cf GLRendertarget().
    GLFramebuffer( const int w, const int h, const unsigned int buffer_bits )
        :
        GLRendertarget(DRAW, w, h, buffer_bits)
    {}

    //! destructeur.
    ~GLFramebuffer(  ) {}

    //! associe une texture au framebuffer. cf GLRendertarget::attachTexture().
    int attachTexture( const unsigned int buffer, GLTexture *texture )
    {
        return GLRendertarget::attachTexture(DRAW, buffer, texture);
    }

    //! valide la configuration du framebuffer.
    int validate( )
    {
        return GLRendertarget::validate(DRAW);
    }
    
    //! utilisation interne. active le framebuffer.
    int bind( )
    {
        return GLRendertarget::bind(DRAW);
    }
    //! utilisation interne. desactive le framebuffer.
    int unbind( )
    {
        return GLRendertarget::unbind(DRAW);
    }
    
    //! definit la texture associee au draw buffer. (destination du varying d'indice 'draw_buffer' du fragment shader). texture 2d. cf GLRendertarget::setBuffer().
    int setDrawbuffer( const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 )
    {
        return setBuffer(DRAW, draw_buffer, buffer, level);
    }
    //! definit la texture associee au draw buffer. (destination du varying d'indice 'draw_buffer' du fragment shader). texture cube. cf GLRendertarget::setFaceBuffer().
    int setFaceDrawbuffer( const GLenum face, const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 )
    {
        return setFaceBuffer(DRAW, face, draw_buffer, buffer, level);
    }
    //! definit la texture associee au draw buffer. (destination du varying d'indice 'draw_buffer' du fragment shader). texture 2d array. cf GLRendertarget::setLayerBuffer().
    int setLayerDrawbuffer( const unsigned int layer, const unsigned int draw_buffer, const unsigned int buffer, const unsigned int level= 0 )
    {
        return setLayerBuffer(DRAW, layer, draw_buffer, buffer, level);
    }
    
    //! met a jour les mipmaps de la texture attachee au framebuffer apres un rendu.
    int updateMipmap( const unsigned int buffer, const unsigned int unit= 0 )
    {
        return updateBufferMipmap(buffer, unit);
    }
    
    //! desactive le draw buffer.
    int resetDrawbuffer( const unsigned int draw_buffer )
    {
        return resetBuffer(DRAW, draw_buffer);
    }
    
    //! definit le buffer de lecture, lorsque le framebuffer est utilise comme Readbuffer, cf gk::setReadFramebuffer(). texture 2d.
    int setReadbuffer( const unsigned int buffer, const unsigned int level= 0 )
    {
        return setBuffer(READ, DRAW0, buffer, level);
    }
    
    //! definit le buffer de lecture, lorsque le framebuffer est utilise comme Readbuffer, cf gk::setReadFramebuffer(). texture cube.
    int setFaceReadbuffer( const GLenum face, const unsigned int buffer, const unsigned int level= 0 )
    {
        return setFaceBuffer(READ, face, DRAW0, buffer, level);
    }
    
    //! definit le buffer de lecture, lorsque le framebuffer est utilise comme Readbuffer, cf gk::setReadFramebuffer(). texture 2d array.
    int setLayerReadbuffer( const unsigned int layer, const unsigned int buffer, const unsigned int level= 0 )
    {
        return setLayerBuffer(READ, layer, DRAW0, buffer, level);
    }    
};

//! representation d'un (read) framebuffer.
class GLReadFramebuffer : public GLRendertarget
{
public:
    //! constructeur.
    GLReadFramebuffer( )
        :
        GLRendertarget()
    {}
    
    //! constructeur. cf GLRendertarget::GLRendertarget( ).
    GLReadFramebuffer( const int w, const int h, const unsigned int buffer_bits )
        :
        GLRendertarget(READ, w, h, buffer_bits)
    {}

    //! destructeur.
    ~GLReadFramebuffer(  ) {}

    //! associe une texture au framebuffer.
    int attachTexture( const unsigned int buffer, GLTexture *texture )
    {
        return GLRendertarget::attachTexture(READ, buffer, texture);
    }
    
    //! verifie la configuration du framebuffer.
    int validate( )
    {
        return GLRendertarget::validate(READ);
    }
    
    //! utilisation interne. active le framebuffer.
    int bind( )
    {
        return GLRendertarget::bind(READ);
    }
    
    //! utilisation interne. desactive le framebuffer.
    int unbind( )
    {
        return GLRendertarget::unbind(READ);
    }
    
    //! definit le buffer de lecture, cf BlitFramebuffer, par exemple. texture 2d.
    int setReadbuffer( const unsigned int buffer, const unsigned int level= 0 )
    {
        return GLRendertarget::setBuffer(READ, DRAW0, buffer, level);
    }
    
    //! definit le buffer de lecture, texture cube.
    int setFaceReadbuffer( const GLenum face, const unsigned int buffer, const unsigned int level= 0 )
    {
        return GLRendertarget::setFaceBuffer(READ, face, DRAW0, buffer, level);
    }
    
    //! definit le buffer de lecture, texture 2d array.
    int setLayerReadbuffer( const unsigned int layer, const unsigned int buffer, const unsigned int level= 0 )
    {
        return GLRendertarget::setLayerBuffer(READ, layer, DRAW0, buffer, level);
    }
};

}

#endif
