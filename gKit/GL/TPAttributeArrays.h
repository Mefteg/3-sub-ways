
#ifndef _TP_ATTRIBUTE_ARRAYS_H
#define _TP_ATTRIBUTE_ARRAYS_H

#include "GLResource.h"
#include "GL/GLPlatform.h"
#include "GL/TPAttributes.h"


namespace gk {

//! representation d'un VertexArray openGL. ensemble de buffers permettant de dessiner un objet.
class GLBufferArray : public GLResource
{
    GLuint m_name;

    // non copyable
    GLBufferArray( const GLBufferArray& );
    GLBufferArray& operator= ( const GLBufferArray& );
    
public:
    //! constructeur par defaut.
    GLBufferArray( )
        :
        GLResource()
    {
        glGenVertexArrays(1, &m_name);
    }
    
    //! destructeur.
    ~GLBufferArray( )
    {
        glDeleteVertexArrays(1, &m_name);
    }
    
    //! associe le contenu d'un buffer aux attributs de sommet, cf. GLBuffer::bindAsVertexAttribute() pour la description des parametres.
    //! index identifiant de l'attribut declare dans le shader, cf. glGetAttribLocation(),
    int setVertexBuffer( const ProgramAttribute& attribute,
        GLAttributeBuffer *buffer,
        const int size, const GLenum type,
        const unsigned long int stride= 0, const unsigned long int offset= 0 )
    {
        if(buffer == NULL)
            return -1;
        if(m_name == 0)
            return -1;
        
        glBindVertexArray(m_name);
        //! \todo
        //~ ActiveAttributes[attribute.location()].setBufferState(attribute, buffer, BufferLayout(size, type, stride, offset));
        return 0;
    }
    
    //! associe le contenu d'un buffer aux attributs d'instance, cf. GLBuffer::bindAsInstanceAttribute() pour la description des parametres.
    //! index identifiant de l'attribut declare dans le shader, cf. glGetAttribLocation(),
    int setInstanceBuffer( const ProgramAttribute& attribute,
        GLAttributeBuffer *buffer, 
        const int size, const GLenum type,
        const unsigned long int stride= 0, const unsigned long int offset= 0,
        const int divisor= 1 )
    {
        if(buffer == NULL)
            return -1;
        if(m_name == 0)
            return -1;
        
        glBindVertexArray(m_name);
        //! \todo
        //~ ActiveAttributes[attribute.location()].setBufferState(attribute, buffer, BufferLayout(size, type, stride, offset, divisor));
        return 0;
    }
    
    int setIndexBuffer( GLIndexBuffer *buffer, const GLenum type= GL_UNSIGNED_INT, const unsigned int offset= 0 )
    {
        if(buffer == NULL)
            return -1;
        if(m_name == 0)
            return -1;
        
        glBindVertexArray(m_name);
        //! \todo
        //~ ActiveIndex.setIndexState(buffer, IndexLayout(type, offset));
        return 0;
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
};

int setBufferArray( GLBufferArray *array );
int resetBufferArray( );

}

#endif
