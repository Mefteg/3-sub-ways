
#include <cstdio>


#include "GLManager.h"
#include "GL/TPFramebuffer.h"
#include "GL/TPTextureUnits.h"

namespace gk {

FramebufferState Framebuffers[2];
FramebufferState ActiveFramebuffers[2];

int FramebufferState::init( )
{
    ActiveFramebuffers[READ]= FramebufferState( READ, GL_READ_FRAMEBUFFER );
    ActiveFramebuffers[DRAW]= FramebufferState( DRAW, GL_DRAW_FRAMEBUFFER );
    Framebuffers[READ]= ActiveFramebuffers[READ];
    Framebuffers[DRAW]= ActiveFramebuffers[DRAW];
    
    return 0;
}

FramebufferState& FramebufferState::operator= ( const FramebufferState& b )
{
    m_framebuffer= b.m_framebuffer;
    m_target= b.m_target;
    m_index= b.m_index;
    return *this;
}

FramebufferState& setFramebuffer( GLRendertarget *framebuffer )
{
    if(framebuffer != NULL)
        framebuffer->bind(DRAW);
    Framebuffers[DRAW]= ActiveFramebuffers[DRAW];
    return Framebuffers[DRAW];
}

FramebufferState& resetFramebuffer( )
{
    ActiveFramebuffers[DRAW].reset();
    Framebuffers[DRAW]= ActiveFramebuffers[DRAW];
    return Framebuffers[DRAW];
}

FramebufferState& setReadFramebuffer( GLRendertarget *framebuffer )
{
    if(framebuffer != NULL)
        framebuffer->bind(READ);
    Framebuffers[READ]= ActiveFramebuffers[READ];
    return Framebuffers[READ];
}

FramebufferState& resetReadFramebuffer( )
{
    ActiveFramebuffers[READ].reset();
    Framebuffers[READ]= ActiveFramebuffers[READ];
    return Framebuffers[READ];
}    

FramebufferState& FramebufferState::setFramebuffer( GLRendertarget *framebuffer )
{
    assert(m_index != -1);
    
    if(framebuffer == NULL)
        return reset();
    
    if(m_framebuffer != framebuffer)
        glBindFramebuffer(m_target, framebuffer->name());
    m_framebuffer= framebuffer;
    return *this;
}

FramebufferState& FramebufferState::reset( )
{
    assert(m_index != -1);
    
    glBindFramebuffer(m_target, 0);
    m_framebuffer= NULL;
    return *this;
}


GLRendertarget::GLRendertarget( )
    :
    m_textures(LAST, NULL),
    m_draw_buffers(LAST, GL_NONE),
    m_draw_buffers_count(0),
    m_width(0),
    m_height(0),
    m_name(0)
{
    glGenFramebuffers(1, &m_name);
}

GLRendertarget::GLRendertarget( const unsigned int state, const int w, const int h, const unsigned int buffer_bits )
    :
    m_textures(LAST, NULL),
    m_draw_buffers(LAST, GL_NONE),
    m_draw_buffers_count(0),
    m_width(w),
    m_height(h),
    m_name(0)
{
    glGenFramebuffers(1, &m_name);
    ActiveFramebuffers[state].setFramebuffer(this);
    
    // parcourir tous les bits : enumerer les textures couleurs a creer / initialiser.
    for(unsigned int bit= 0; bit < LAST_COLOR; bit++)
    {
        if((buffer_bits & (1<<bit)) == 0)
            continue;
        
        GLTexture2D *color= new GLTexture2D(gk::UNIT0, w, h);   // creation arbitraire sur texture unit 0
        if(color == NULL || color->createGLResource() < 0)
            return;
        
        m_textures[bit]= GLManager<GLTexture2D>::manager().insert(color);
        m_draw_buffers[m_draw_buffers_count]= GL_COLOR_ATTACHMENT0 + m_draw_buffers_count;
        
        glFramebufferTexture2D(ActiveFramebuffers[state].target(), 
            GL_COLOR_ATTACHMENT0 + m_draw_buffers_count, color->target(), color->name(), 0);
        m_draw_buffers_count++;
    }
    
    // cree un zbuffer, si necessaire.
    if(buffer_bits & DEPTH_BIT)
    {
        GLDepthTexture *depth= new GLDepthTexture(gk::UNIT1, w, h);     // creation arbitraire sur texture unit 1
        if(depth == NULL || depth->createGLResource() < 0)
            return;
        
        glFramebufferTexture2D(ActiveFramebuffers[state].target(), 
            GL_DEPTH_ATTACHMENT, depth->target(), depth->name(), 0);
        m_textures[DEPTH]= GLManager<GLDepthTexture>::manager().insert(depth);
    }
    
    // verifie la configuration du frmaebuffer.
    GLenum status= (GLenum) glCheckFramebufferStatus(ActiveFramebuffers[state].target());
    if(status != GL_FRAMEBUFFER_COMPLETE)
        printf("GLFramebuffer: error.\n");
    
    // desactive le nouveau framebuffer.
    ActiveFramebuffers[state].reset();
}

GLRendertarget::~GLRendertarget(  )
{
    if(m_name != 0)
        glDeleteFramebuffers(1, &m_name);
}

int GLRendertarget::attachTexture( const unsigned int state, const unsigned int buffer, GLTexture *texture )
{
    if(m_name == 0)
        return -1;
    if(buffer >= LAST)
        return -1;
    if(texture == NULL)
        return -1;

#ifdef VERBOSE_DEBUG
    if(buffer < LAST_COLOR)
        printf("GLFramebuffer::attachTexture( ): buffer 0x%x, texture %d.\n", 
            GL_COLOR_ATTACHMENT0 + buffer, texture->name());
    else if(buffer == DEPTH)
        printf("GLFramebuffer::attachTexture( ): buffer 0x%x, texture %d\n.", 
            GL_DEPTH_ATTACHMENT, texture->name());
#endif
    
    // verifie les dimensions de la texture
    if(m_width > 0 && texture->width() != m_width)
        return -1;
    m_width= texture->width();
    if(m_height > 0 && texture->height() != m_height)
        return -1;
    m_height= texture->height();
    
    // attache la texture
    m_textures[buffer]= texture;
    if(buffer >= LAST_COLOR)
        return 0;

    // recompte le nombre de draw buffers couleur
    m_draw_buffers_count= 0;
    for(int i= 0; i < LAST_COLOR; i++)
        if(m_textures[i]  != NULL)
            m_draw_buffers_count++;
    
    return 0;
}

GLTexture *GLRendertarget::texture( const unsigned int buffer )
{
    if(m_name == 0)
        return NULL;
    if(buffer >= LAST)
        return NULL;
    
    return m_textures[buffer];
}

int GLRendertarget::updateBufferMipmap( const unsigned int buffer, const unsigned int unit )
{
    if(m_name == 0)
        return -1;
    if(buffer >= LAST)
        return -1;
    if(m_textures[buffer] == NULL)
        return -1;

    ActiveTextureUnits[unit].setTexture( ProgramSampler(unit), m_textures[buffer]);
    glGenerateMipmap(m_textures[buffer]->target());
    return 0;
}

int GLRendertarget::setBuffer( const unsigned int state, const unsigned int index, const unsigned int buffer, const unsigned int level )
{
    if(m_name == 0)
        return -1;
    if(index >= LAST)
        return -1;
    if(buffer >= LAST || m_textures[buffer] == NULL)
        return -1;
    
    unsigned int attach= 0;
    if(buffer < LAST_COLOR)
    {
        attach= GL_COLOR_ATTACHMENT0 + buffer;
        m_draw_buffers[index]= attach;
    }
    else if(buffer == DEPTH)
        attach= GL_DEPTH_ATTACHMENT;
    else
        return -1;
    
    ActiveFramebuffers[state].setFramebuffer(this);
    glFramebufferTexture2D(ActiveFramebuffers[state].target(), 
        attach, m_textures[buffer]->target(), m_textures[buffer]->name(), level);
    assert(glGetError() == GL_NO_ERROR);
    
#ifdef VERBOSE_DEBUG
    printf("GLFramebuffer::setDrawbuffer(0x%x): texture %d, level %d, %dx%d.\n", 
        attach, m_textures[buffer]->name(), level,
        m_textures[buffer]->width() / (1<<level), m_textures[buffer]->height() / (1<<level));
#endif
    return 0;
}

int GLRendertarget::resetBuffer( const unsigned int state, const unsigned int index )
{
    if(m_name == 0)
        return -1;
    if(index >= LAST)
        return -1;
    
    unsigned int attach= 0;
    if(index < LAST_COLOR)
        attach= GL_COLOR_ATTACHMENT0 + index;
    else if(index == DEPTH)
        attach= GL_DEPTH_ATTACHMENT;
    else
        return -1;
    
    ActiveFramebuffers[state].setFramebuffer(this);
    GLenum target= (m_textures[index] != NULL) ?  m_textures[index]->target() : GL_TEXTURE_2D;  // ?? a verifier
    glFramebufferTexture2D(ActiveFramebuffers[state].target(), 
        attach, target, 0, 0);
    return 0;
}


int GLRendertarget::setFaceBuffer( const unsigned int state, const GLenum face, const unsigned int index, const unsigned int buffer, const unsigned int level )
{
    if(m_name == 0)
        return -1;
    if(index >= LAST)
        return -1;
    if(buffer >= LAST || m_textures[buffer] == NULL)
        return -1;

    unsigned int attach= 0;
    if(buffer < LAST_COLOR)
    {
        attach= GL_COLOR_ATTACHMENT0 + buffer;
        m_draw_buffers[index]= attach;
    }
    else if(buffer == DEPTH)
        attach= GL_DEPTH_ATTACHMENT;
    else
        return -1;
    
    ActiveFramebuffers[state].setFramebuffer(this);
    glFramebufferTexture2D(ActiveFramebuffers[state].target(), 
        attach, face, m_textures[buffer]->name(), level);
    return 0;
}

int GLRendertarget::setLayerBuffer( const unsigned int state, const unsigned int layer, const unsigned int index, const unsigned int buffer, const unsigned int level )
{
    if(m_name == 0)
        return -1;
    if(index >= LAST)
        return -1;
    if(buffer >= LAST || m_textures[buffer] == NULL)
        return -1;

    unsigned int attach= 0;
    if(buffer < LAST_COLOR)
    {
        attach= GL_COLOR_ATTACHMENT0 + buffer;
        m_draw_buffers[index]= attach;
    }
    else if(buffer == DEPTH)
        attach= GL_DEPTH_ATTACHMENT;
    else
        return -1;
    
    ActiveFramebuffers[state].setFramebuffer(this);
    glFramebufferTextureLayer(ActiveFramebuffers[state].target(), 
        attach, m_textures[buffer]->name(), level, layer);
    return 0;
}

int GLRendertarget::validate( const unsigned int state )
{
    assert(glGetError() == GL_NO_ERROR);

    ActiveFramebuffers[state].setFramebuffer(this);
    GLenum status= (GLenum) glCheckFramebufferStatus(ActiveFramebuffers[state].target());
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("GLFramebuffer::validate( ) error: ");
        switch(status)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
                printf("framebuffer undefined");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                printf("framebuffer incomplete attachment");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                printf("framebuffer incomplete missing attachment");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                printf("framebuffer incomplete draw buffer");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                printf("framebuffer unsupported");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                printf("framebuffer incomplete layer targets");
                break;
            
            default:
                printf("??");
                break;
        }
        
        printf("\n");
        return -1;
    }
    
    return 0;
}

int GLRendertarget::bind( const unsigned int state )
{
    assert(glGetError() == GL_NO_ERROR);
    if(m_name == 0)
        return -1;
    
    ActiveFramebuffers[state].setFramebuffer(this);
    if(state == READ)
        glReadBuffer(GL_COLOR_ATTACHMENT0);     // et DEPTH ?
    else
        glDrawBuffers(m_draw_buffers_count, &m_draw_buffers.front());
    assert(glGetError() == GL_NO_ERROR);
    
    #ifdef VERBOSE_DEBUG
    {
        for(int i= 0; i < m_draw_buffers_count; i++)
            printf("GLFramebuffer::bind( ): draw buffer %d: 0x%x\n", i, m_draw_buffers[i]);
    }
    #endif
    
    return 0;
}

int GLRendertarget::unbind( const unsigned int state )
{
    ActiveFramebuffers[state].reset();
    return 0;
}

int BlitFramebuffer( const int sx_min, const int sy_min, const int sx_max, const int sy_max,
    const int dx_min, const int dy_min, const int dx_max, const int dy_max, 
    const GLbitfield buffer_bits, const GLenum filter )
{
    glBlitFramebuffer(sx_min, sy_min, sx_max, sy_max, dx_min, dy_min, dx_max, dy_max, buffer_bits, filter);
    return 0;
}

}
