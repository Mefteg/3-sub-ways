
#ifndef _TP_BUFFER_H
#define _TP_BUFFER_H

#include <cstdio>
#include <cassert>

#include "GLResource.h"
#include "GL/GLPlatform.h"
#include "GL/TPProgramName.h"
#include "GL/TPAttributes.h"


namespace gk {

//! utilisation interne. representation d'un buffer openGL.
class GLBuffer : public GLResource
{
protected:
    GLuint m_name;
    GLenum m_usage;
    unsigned int m_length;
    unsigned int m_count;

    // non copyable
    GLBuffer( const GLBuffer& );
    GLBuffer& operator=( const GLBuffer& );
    
    //! constructeur d'un buffer, un tableau de vecteurs de 'count' elements.
    //! \param count nombre de vecteurs,
    //! \param length longueur totale du buffer en OCTETS,
    //! \param data pointeur sur les donnees a transferer dans le buffer, ou NULL pour initialiser un buffer vide,
    //! \param usage decrit le type d'utilisation du buffer.
    GLBuffer( const int state, 
        const unsigned int count, const unsigned int length, const void *data, const GLenum usage= GL_STATIC_DRAW )
        :
        m_name(0),
        m_usage(usage),
        m_length(length),
        m_count(count)
    {
        glGenBuffers(1, &m_name);
        
        ActiveAttributes[state].setBuffer(this);
        //~ glBindBuffer(ActiveAttributes[state].target(), m_name);
        glBufferData(ActiveAttributes[state].target(), m_length, data, m_usage);
    }

    //! efface le contenu du buffer.
    int clear( const int state )
    {
        if(m_name == 0)
            return -1;
        
        ActiveAttributes[state].setBuffer(this);
        //~ glBindBuffer(ActiveAttributes[state].target(), m_name);
        glBufferData(ActiveAttributes[state].target(), m_length, NULL, m_usage);
        return 0;
    }

    //! modifie le contenu d'une partie du buffer.
    //! \param offset position du premier octet a modifier,
    //! \param length nombre d'octets a modifier.
    int update( const int state, 
        const unsigned long int offset, const unsigned long int length, const void *data )
    {
        if(m_name == 0)
            return -1;
        if(offset + length > m_length)
            return -1;
        
        ActiveAttributes[state].setBuffer(this);
        glBufferSubData(ActiveAttributes[state].target(), offset, length, data);
        return 0;
    }
    
    //! mappe le contenu du buffer en memoire host
    void *map( const int state, 
        const unsigned long int offset, const unsigned int length, const GLbitfield access )
    {
        if(m_name == 0)
            return NULL;
        if(offset + length > m_length)
        {
            printf("GLBuffer::map( ): offset + length > buffer length\n");
            return NULL;
        }
        
        ActiveAttributes[state].setBuffer(this);
        //~ glBindBuffer(target, m_name);
        return glMapBufferRange(ActiveAttributes[state].target(), offset, length, access);
    }
    
    int unmap( const int state )
    {
        if(m_name == 0)
            return -1;
        
        ActiveAttributes[state].setBuffer(this);
        glUnmapBuffer(ActiveAttributes[state].target());
        return 0;
    }
    
    int flush( const int state, 
        const unsigned long int offset, const unsigned int length )
    {
        if(m_name == 0)
            return -1;
        if(offset + length > m_length)
        {
            printf("GLBuffer::flush( ): offset + length > buffer length\n");
            return -1;
        }
        
        ActiveAttributes[state].setBuffer(this);
        //~ glBindBuffer(target, m_name);
        glFlushMappedBufferRange(ActiveAttributes[state].target(), offset, length);
        return 0;
    }
    
public: // temp
    //! utilise le buffer comme destination du feedback pour le varying 'index' declare dans le shader, cf glTransformFeedbackVaryings().
    int bindAsFeedbackAttribute( const ProgramFeedback& attribute )
    {
        if(m_name == 0)
            return -1;
        if(attribute.index() < 0)
            return -1;

        //~ ActiveAttributes[FEEDBACK0 + attribute.index()].setBuffer(this);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, attribute.index(), m_name);
        return 0;
    }

#ifdef GK_OPENGL4
    //! utilise le buffer comme tableau de draw calls, cf. opengl4 glDrawElementsIndirect(), glDrawArraysIndirect().
    int bindAsIndirect( )
    {
        if(m_name == 0)
            return -1;

        //~ ActiveAttributes[INDIRECT].setBuffer(this);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_name);
        return 0;
    }
#endif

    //! utilise le contenu du buffer comme tableau d'index pour un affichage avec sommets partages. cf. glDrawElements().
    int bindAsIndex( )
    {
        if(m_name == 0)
            return -1;

        ActiveAttributes[INDEX].setBuffer(this);
        //~ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_name);
        return 0;
    }
        
public:    
    //! destructeur.
    virtual ~GLBuffer( )
    {
        if(m_name != 0)
            glDeleteBuffers(1, &m_name);
    }

    //! creation de la ressource openGL.
    int createGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }

    //! destruction de la ressource openGL.
    int releaseGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }

    //! renvoie le nom openGL (l'identifiant) du buffer.
    GLuint name( ) const
    {
        return m_name;
    }

    //! renvoie le nombre de vecteurs alloues (valeur passee au contructeur, cf GLBuffer()).
    unsigned int count( ) const
    {
        if(m_name == 0)
            return 0;
        return m_count;
    }

    static
    void unbindIndex( )
    {
        // rien a faire, le buffer ne sera utilise que par un draw indexe.
    }
    
    static
    void unbindVertexAttribute( const ProgramAttribute& attribute )
    {
        if(attribute.location() < 0)
            return;
        glDisableVertexAttribArray(attribute.location());
    }

    static
    void unbindInstanceAttribute( const ProgramAttribute& attribute )
    {
        if(attribute.location() < 0)
            return;
        glDisableVertexAttribArray(attribute.location());
    }

    static
    void unbindFeedbackAttribute( const ProgramFeedback& attribute )
    {
        //~ glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, 0);
        // rien a faire le buffer ne sera pas utilise en dehors de glBeginTransformFeedback() / glEndTransformFeedback()
    }

#ifdef GK_OPENGL4
    static
    void unbindIndirect( )
    {
        // rien a faire le buffer ne sera utilise que par glDrawXXXIndirect( ).
    }
#endif
};

//! representation d'un buffer d'attribut.
class GLAttributeBuffer : public GLBuffer
{
public:
    //! constructeur.
    GLAttributeBuffer( const unsigned int count, const unsigned int length, const void *data, const GLenum usage= GL_STATIC_DRAW )
        :
        GLBuffer(ATTRIBUTE, count, length, data, usage)
    {}
    
    //! destructeur.
    ~GLAttributeBuffer( ) {}
    
    //! efface le contenu du buffer.
    int clear( )
    {
        return GLBuffer::clear(ATTRIBUTE);
    }
    
    //! remplace une partie de contenu du buffer.
    int update( const unsigned long int offset, const unsigned long int length, const void *data )
    {
        return GLBuffer::update(ATTRIBUTE, offset, length, data );
    }
    
    //! mappe une partie du buffer en memoire centrale.
    void *map( const unsigned long int offset, const unsigned long int length, const GLbitfield access )
    {
        return GLBuffer::map(ATTRIBUTE, offset, length, access );
    }
    
    //! termine le mapping du contenu du buffer en memoire centrale, les donnees modifiees sont transferees sur le gpu.
    int unmap( )
    {
        return GLBuffer::unmap(ATTRIBUTE);
    }
    
    //! termine une partie du mapping du contenu du buffer en memoire centrale, les donnees modifiiees sont transferees sur le gpu.
    int flush( const unsigned long int offset, const unsigned long int length )
    {
        return GLBuffer::flush(ATTRIBUTE, offset, length);
    }
};

//! representation d'un buffer d'indexation.
class GLIndexBuffer : public GLBuffer
{
public:
    GLIndexBuffer( const unsigned int count, const unsigned int length, const void *data, const GLenum usage= GL_STATIC_DRAW )
        :
        GLBuffer(INDEX, count, length, data, usage )
    {}
    
    ~GLIndexBuffer( ) {}
    
    int clear( )
    {
        return GLBuffer::clear(INDEX);
    }
    
    int update( const unsigned long int offset, const unsigned long int length, const void *data )
    {
        return GLBuffer::update(INDEX, offset, length, data );
    }
    
    void *map( const unsigned long int offset, const unsigned long int length, const GLbitfield access )
    {
        return GLBuffer::map(INDEX, offset, length, access );
    }
    
    int unmap( )
    {
        return GLBuffer::unmap(INDEX);
    }
    
    int flush( const unsigned long int offset, const unsigned long int length )
    {
        return GLBuffer::flush(INDEX, offset, length);
    }
};

}

#endif
