
#ifndef _GLQUERY_H
#define _GLQUERY_H

#include "GL/GLPlatform.h"


namespace gk {
    
//! utilisation interne. representation d'un objet requete.
class GLQuery : public GLResource
{
    GLuint m_name;      //!< identifiant de la requete
    GLenum m_target;    //!< type de la requete GL_TIME_ELAPSED, GL_SAMPLES_PASSED, etc.
    GLint m_bits_n;     //!< taille du resultat : 32 ou 64 bits.
    
public:
    //! constructeur.
    GLQuery( const GLenum target ) 
        :
        m_name(0),
        m_target(target),
        m_bits_n(0)
    {
        glGenQueries(1, &m_name);
        glGetQueryiv(m_target, GL_QUERY_COUNTER_BITS, &m_bits_n);
    #ifdef VERBOSE_DEBUG
        printf("GLQuery( ): %d bits result\n", m_bits_n);
    #endif
    }
    
    //! destructeur.
    ~GLQuery( )
    {
        glDeleteQueries(1, &m_name);
    }
    
    //! creation de l'objet requete.
    int createGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    //! destruction de l'objet requete.
    int releaseGLResource( )
    {
        return (m_name != 0) ? 0 : -1;
    }
    
    GLuint name( ) const
    {
        return m_name;
    }
    
    //! debut de la requete, remise a 0 du compteur associe.
    int begin( )
    {
        if(m_name == 0)
            return -1;
        glBeginQuery(m_target, m_name);
        return 0;
    }
    
    //! fin de la requete. le resultat n'est pas necessairement disponible, cf. isAvailable().
    int end( ) 
    {
        if(m_name == 0)
            return -1;
        glEndQuery(m_target);
        return 0;
    }
    
    //! determine si le resultat de la requete est disponible.
    bool isAvailable( )
    {
        if(m_name == 0)
            return -1;
        GLint available= 0;
        glGetQueryObjectiv(m_name, GL_QUERY_RESULT_AVAILABLE, &available);
        return (available == GL_TRUE);
    }
    
    //! renvoie vrai si le resultat est code sur 64 bits.
    int is64bits( )
    {
        if(m_name == 0)
            return -1;
        return (m_bits_n == 64) ? 1 : 0;
    }
    
    //! renvoie le resultat de la requete (sur 32 bits), attends le resultat, s'il n'est pas disponible.
    int result( )
    {
        if(m_name == 0)
            return -1;
        
        GLint n= 0;
        glGetQueryObjectiv(m_name, GL_QUERY_RESULT, &n);
        return n;
    }
    
    //! renvoie le resultat de la requete (sur 64 bits), attends le resultat, s'il n'est pas disponible.
    long long int result64( )
    {
        if(m_name == 0)
            return -1;
        
        GLint64 n= 0;
        glGetQueryObjecti64v(m_name, GL_QUERY_RESULT, &n);
        return n;
    }
};

//! temps gpu necessaire a l'execution des commandes entre begin() et end().
class GLTimer : public GLQuery
{
public:
    GLTimer( )
        :
        GLQuery(GL_TIME_ELAPSED)
    {}
    
    ~GLTimer( ) {}
};

//! nombre de fragments ecrits entre begin() et end().
class GLOcclusion : public GLQuery
{
public:
    GLOcclusion( )
        :
        GLQuery(GL_SAMPLES_PASSED)
    {}
    
    ~GLOcclusion( ) {}
};

//! nombre de primitives emises en sortie du vertex shader, ou du geometrie shader, s'il existe.
class GLPrimitivesGenerated : public GLQuery
{
public:
    GLPrimitivesGenerated( )
        :
        GLQuery(GL_PRIMITIVES_GENERATED)
    {}
    
    ~GLPrimitivesGenerated( ) {}
};

//! nombre de primitives ecrites dans les feedback buffers.
class GLFeedbackPrimitives : public GLQuery
{
public:
    GLFeedbackPrimitives( )
        :
        GLQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
    {}
    
    ~GLFeedbackPrimitives( ) {}
};

}

#endif
