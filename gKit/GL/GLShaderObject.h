
#ifndef _GLSHADER_OBJECT_H
#define _GLSHADER_OBJECT_H

#include <vector>
#include <string>

#include "GLPlatform.h"
#include "GLResource.h"
#include "TextFile.h"
#include "TextFileIO.h"


namespace gk {

class GLShaderProgram;
class GLContext;

//! utilisation interne. abstraction d'un shader object, ressource openGL.
class GLShaderObject : public GLResource
{
protected:
    TextFile *m_text;

    GLenum m_type;
    GLuint m_name;
    bool m_errors;

    GLShaderObject( const GLShaderObject& );
    GLShaderObject& operator= ( const GLShaderObject& );

public:
    friend class GLShaderProgram;
    friend class GLContext;

    //! constructeur.
    GLShaderObject( const GLenum type )
        :
        m_text(NULL),
        m_type(type),
        m_name(0),
        m_errors(false)
    {}

    //! destructeur.
    virtual ~GLShaderObject( )
    {
        delete m_text;
    }

    //! ajoute une definition au source. "#define 'definition' 'value'".
    int pushDefinition( const std::string& definition, const std::string& value= "" )
    {
        if(m_text == NULL)
            return -1;
        return m_text->define(definition, value);
    }

    //! ajoute le contenu d'un fichier source.
    int pushSource( const TextFile *text )
    {
        if(text == NULL)
            return 0;
        if(m_text == NULL)
            m_text= new TextFile(text->name());
        if(m_text == NULL)
            return -1;

        return m_text->include(text);
    }

    //! affiche les sources du shader.
    void printSource( ) const
    {
        if(m_text == NULL)
            return;
        m_text->print();
    }

    //! affiche les erreurs de compilation du shader.
    void printErrors( );

    //! cree l'objet openGL.
    int createGLResource( );
    //! detruit l'objet openGL.
    int releaseGLResource( );

    //! renvoie vrai ou faux, si erreurs de lecture du source.
    bool errors( ) const
    {
        return m_errors;
    }

    //! renvoie le type openGL du shader.
    GLenum type( ) const
    {
        return m_type;
    }

    //! renvoie l'identifiant du shader object openGL.
    GLuint name( ) const
    {
        return m_name;
    }
};


//! representation d'un vertex shader object, objet openGL.
class GLVertexShader : public GLShaderObject
{
public:
    //! constructeur par defaut.
    GLVertexShader( )
        :
        GLShaderObject(GL_VERTEX_SHADER)
    {}

    //! construction a partir du contenu d'un fichier source.
    GLVertexShader( const TextFile *file )
        :
        GLShaderObject(GL_VERTEX_SHADER)
    {
        pushSource(file);
    }

    //! construction a partir du nom d'un fichier source.
    GLVertexShader( const std::string& filename )
        :
        GLShaderObject(GL_VERTEX_SHADER)
    {
    #ifdef VERBOSE
        printf("loading vertex shader '%s'...\n", filename.c_str());
    #endif
        TextFile *source= TextFileIO::read(filename, "vertex");
        if(filename.empty() == false && source == NULL)
        {
            printf("GLVertexShader( ): error reading source '%s'.\n",
                filename.c_str());
            m_errors= true;
        }

        pushSource(source);
    }

    //! destructeur.
    ~GLVertexShader( ) {}
};

//! representation d'un fragment shader object, objet openGL.
class GLFragmentShader: public GLShaderObject
{
public:
    //! constructeur par defaut.
    GLFragmentShader( )
        :
        GLShaderObject(GL_FRAGMENT_SHADER)
    {}

    //! construction a partir du contenu d'un fichier source.
    GLFragmentShader( const TextFile *file )
        :
        GLShaderObject(GL_FRAGMENT_SHADER)
    {
        pushSource(file);
    }

    //! construction a partir du nom d'un fichier source.
    GLFragmentShader( const std::string& filename )
        :
        GLShaderObject(GL_FRAGMENT_SHADER)
    {
    #ifdef VERBOSE
        printf("loading fragment shader '%s'...\n", filename.c_str());
    #endif
        TextFile *source= TextFileIO::read(filename, "fragment");
        if(filename.empty() == false && source == NULL)
        {
            printf("GLFragmentShader( ): error reading source '%s'.\n",
                filename.c_str());
            m_errors= true;
        }

        pushSource(source);
    }

    //! destructeur.
    ~GLFragmentShader( ) {}
};


#if defined GK_OPENGL3 || defined GK_OPENGL4
//! representation d'un geometry shader object. objet openGL.
class GLGeometryShader : public GLShaderObject
{
public:
    GLGeometryShader( )
        :
        GLShaderObject(GL_GEOMETRY_SHADER)
    {}

    GLGeometryShader( const TextFile *file )
        :
        GLShaderObject(GL_GEOMETRY_SHADER)
    {
        pushSource(file);
    }

    GLGeometryShader( const std::string& filename )
        :
        GLShaderObject(GL_GEOMETRY_SHADER)
    {
    #ifdef VERBOSE
        printf("loading geometry shader '%s'...\n", filename.c_str());
    #endif
        TextFile *source= TextFileIO::read(filename, "geometry");
        if(filename.empty() == false && source == NULL)
        {
            printf("GLGeometryShader( ): error reading source '%s'.\n",
                filename.c_str());
            m_errors= true;
        }

        pushSource(source);
    }
};
#endif

#ifdef GK_OPENGL4
//! representation d'un tesselation control shader object. objet openGL.
class GLControlShader : public GLShaderObject
{
public:
    GLControlShader( )
        :
        GLShaderObject(GL_TESS_CONTROL_SHADER)
    {}

    GLControlShader( const TextFile *file )
        :
        GLShaderObject(GL_TESS_CONTROL_SHADER)
    {
        pushSource(file);
    }

    GLControlShader( const std::string& filename )
        :
        GLShaderObject(GL_TESS_CONTROL_SHADER)
    {
    #ifdef VERBOSE
        printf("loading control shader '%s'...\n", filename.c_str());
    #endif
        TextFile *source= TextFileIO::read(filename, "control");
        if(filename.empty() == false && source == NULL)
        {
            printf("GLControlShader( ): error reading source '%s'.\n",
                filename.c_str());
            m_errors= true;
        }

        pushSource(source);
    }
};

//! representation d'un tesselation evaluation shader object. objet openGL.
class GLEvaluationShader : public GLShaderObject
{
public:
    GLEvaluationShader( )
        :
        GLShaderObject(GL_TESS_EVALUATION_SHADER)
    {}

    GLEvaluationShader( const TextFile *file )
        :
        GLShaderObject(GL_TESS_EVALUATION_SHADER)
    {
        pushSource(file);
    }

    GLEvaluationShader( const std::string& filename )
        :
        GLShaderObject(GL_TESS_EVALUATION_SHADER)
    {
    #ifdef VERBOSE
        printf("loading evaluation shader '%s'...\n", filename.c_str());
    #endif
        TextFile *source= TextFileIO::read(filename, "evaluation");
        if(filename.empty() == false && source == NULL)
        {
            printf("GLEvaluationShader( ): error reading source '%s'.\n",
                filename.c_str());
            m_errors= true;
        }

        pushSource(source);
    }
};

#endif

}       // namespace
#endif
