
#ifndef _GL_STREAM_QUERY_H
#define _GL_STREAM_QUERY_H

#include "GL/GLPlatform.h"


namespace gk {
    
//! utilisation interne. representation d'un objet requete.
class GLStreamQuery : public GLResource
{
    GLuint *m_names;      //!< identifiant de la requete
    GLenum m_target;    //!< type de la requete GL_TIME_ELAPSED, GL_SAMPLES_PASSED, etc.
    unsigned int m_count;       //!< index de la requete / identifiant du flux pour "transform feedback" ou "primitive generated".
    GLint m_bits_n;     //!< taille du resultat : 32 ou 64 bits.
    
public:
    //! constructeur.
    GLStreamQuery( const GLenum target, const unsigned int count ) 
        :
        m_names(NULL),
        m_target(target),
        m_count(count),
        m_bits_n(0)
    {
        m_names= new GLuint [m_count];
        glGenQueries(m_count, m_names);
        glGetQueryiv(m_target, GL_QUERY_COUNTER_BITS, &m_bits_n);
    #ifdef VERBOSE_DEBUG
        printf("GLStreamQuery( ): %d bits result\n", m_bits_n);
    #endif
    }
    
    //! destructeur.
    ~GLStreamQuery( )
    {
        glDeleteQueries(m_count, m_names);
        delete [] m_names;
    }
    
    //! creation de l'objet requete.
    int createGLResource( )
    {
        return (m_names != NULL) ? 0 : -1;
    }
    
    //! destruction de l'objet requete.
    int releaseGLResource( )
    {
        return (m_names != NULL) ? 0 : -1;
    }
    
    //! renvoie le nom opengl d'une requete.
    GLuint name( const unsigned int index ) const
    {
        if(m_names == NULL || index >= m_count)
            return 0;
        return m_names[index];
    }
    
    //! debut de la requete, remise a 0 du compteur associe.
    int begin( const unsigned int index )
    {
        if(m_names == NULL || index >= m_count || m_names[index] == 0)
            return -1;
        
        glBeginQueryIndexed(m_target, index, m_names[index]);
        return 0;
    }
    
    //! fin de la requete. le resultat n'est pas necessairement disponible, cf. isAvailable().
    int end( const unsigned int index ) 
    {
        if(m_names == NULL || index >= m_count || m_names[index] == 0)
            return -1;
        glEndQueryIndexed(m_target, index);
        return 0;
    }
    
    //! determine si le resultat de la requete est disponible.
    bool isAvailable( const unsigned int index )
    {
        if(m_names == NULL || index >= m_count || m_names[index] == 0)
            return -1;
        GLint available= 0;
        
        glGetQueryObjectiv(m_names[index], GL_QUERY_RESULT_AVAILABLE, &available);
        
        #ifdef VERBOSE
        {
            GLint name;
            GLint tmp;
            glGetQueryIndexediv(m_target, index, GL_CURRENT_QUERY, &name);
            glGetQueryObjectiv(name, GL_QUERY_RESULT_AVAILABLE, &tmp);
            printf("GLStreamQuery( ): available: %d (%d).\n", available, tmp);
        }
        #endif
        
        return (available == GL_TRUE);
    }
    
    //! renvoie vrai si le resultat est code sur 64 bits.
    int is64bits( )
    {
        if(m_names == NULL)
            return -1;
        return (m_bits_n == 64) ? 1 : 0;
    }
    
    //! renvoie le resultat de la requete (sur 32 bits), attends le resultat, s'il n'est pas disponible.
    int result( const unsigned int index )
    {
        if(m_names == NULL || index >= m_count || m_names[index] == 0)
            return -1;
        
        GLint n= 0;
        glGetQueryObjectiv(m_names[index], GL_QUERY_RESULT, &n);
        return n;
    }
    
    //! renvoie le resultat de la requete (sur 64 bits), attends le resultat, s'il n'est pas disponible.
    long long int result64( const unsigned int index )
    {
        if(m_names == NULL || index >= m_count || m_names[index] == 0)
            return -1;
        
        GLint64 n= 0;
        glGetQueryObjecti64v(m_names[index], GL_QUERY_RESULT, &n);
        return n;
    }
};

//! nombre de primitives emises en sortie du vertex shader, ou du geometry shader, s'il existe.
class GLStreamPrimitivesGenerated : public GLStreamQuery
{
public:
    GLStreamPrimitivesGenerated( const unsigned int count  )
        :
        GLStreamQuery(GL_PRIMITIVES_GENERATED, count)
    {}
    
    ~GLStreamPrimitivesGenerated( ) {}
};

//! nombre de primitives ecrites dans les feedback buffers.
class GLStreamFeedbackPrimitives : public GLStreamQuery
{
public:
    GLStreamFeedbackPrimitives( const unsigned int count )
        :
        GLStreamQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, count)
    {}
    
    ~GLStreamFeedbackPrimitives( ) {}
};

}

#endif
