
#include <cstring>

#include "GL/GLPlatform.h"
#include "ShaderManager.h"
#include "App.h"


class GLSLCC : public gk::App
{
    int m_argc;
    char **m_argv;
    
    int shader_type( char *fname, GLenum& type )
    {
        char *ext;
        
        if(fname == NULL)
            return -1;
        
        ext= strrchr(fname, '.');
        if(ext == NULL)
            return -1;
        
        if(strchr(ext, 'v') != NULL || strchr(ext, 'V') != NULL)
        {
            type= GL_VERTEX_SHADER;
            return 0;
        }
        
        if(strchr(ext, 'f') != NULL || strchr(ext, 'F') != NULL)
        {
            type= GL_FRAGMENT_SHADER;
            return 0;
        }
        
    #if defined GK_OPENGL3 || defined GK_OPENGL4
        if(strchr(ext, 'g') != NULL || strchr(ext, 'G') != NULL)
        {
            type= GL_GEOMETRY_SHADER;
            return 0;
        }
    #endif
        
    #ifdef GK_OPENGL4
        if(strchr(ext, 'c') != NULL || strchr(ext, 'C') != NULL)
        {
            type= GL_TESS_CONTROL_SHADER;
            return 0;
        }
    #endif
        
    #ifdef GK_OPENGL4
        if(strchr(ext, 'e') != NULL || strchr(ext, 'E') != NULL)
        {
            type= GL_TESS_EVALUATION_SHADER;
            return 0;
        }
    #endif
        
        return -1;
    }
    
public:
    GLSLCC( int argc, char **argv, const int w= 1024, const int h= 768 )
        :
        gk::App(w, h),
        m_argc(argc),
        m_argv(argv)
    {}
    
    ~GLSLCC( ) {}
    
    int init( )
    {
        int code= 0;
        
        if(m_argc > 1 && strcmp(m_argv[1], "-c")==0)
        {
            // compiler separement tous les shaders de la ligne de commande
            for(int i= 2; i < m_argc; i++)
            {
                GLenum type;
                if(shader_type(m_argv[i], type) < 0)
                {
                    printf("  %s: unknown shader type.\n", m_argv[i]);
                    return -1;
                }
                
                if(type == GL_VERTEX_SHADER)
                    {;}
                else if(type == GL_FRAGMENT_SHADER)
                    {;}
            #if defined GK_OPENGL3 || defined GK_OPENGL4
                else if(type == GL_GEOMETRY_SHADER)
                    {;}
            #endif
            #ifdef GK_OPENGL4
                else if(type == GL_TESS_CONTROL_SHADER)
                    {;}
                else if(type == GL_TESS_EVALUATION_SHADER)
                    {;}
            #endif
                else
                    return -1;
                
                gk::GLShaderObject *shader= gk::GLManager<gk::GLShaderObject>::manager().insert(
                        new gk::GLShaderObject(type));
                
                gk::TextFile *source= gk::TextFileIO::read(m_argv[i]);
                if(source == NULL)
                {
                    printf("error reading source '%s'.\n", m_argv[i]);
                    return -1;
                }
                else
                {
                    shader->pushSource(source);
                    code= shader->createGLResource();
                }
            }
        }
        
        else if(m_argc > 1)
        {
            // compiler les shaders et les linker
            gk::GLShaderProgram *program= new gk::GLShaderProgram();

            // identifie le vertex shader et le fragment shader
            for(int i= 1; i < m_argc; i++)
            {
                GLenum type;
                if(shader_type(m_argv[i], type) < 0)
                {
                    printf("  %s: unknown shader type.\n", m_argv[i]);
                    return -1;
                }
                
                printf("  %s (type 0x%x)\n", m_argv[i], type);
                
                if(type == GL_VERTEX_SHADER)
                    {;}
                else if(type == GL_FRAGMENT_SHADER)
                    {;}
            #if defined GK_OPENGL3 || defined GK_OPENGL4
                else if(type == GL_GEOMETRY_SHADER)
                    {;}
            #endif
            #ifdef GK_OPENGL4
                else if(type == GL_TESS_CONTROL_SHADER)
                    {;}
                else if(type == GL_TESS_EVALUATION_SHADER)
                    {;}
            #endif
                else
                    return -1;
                
                gk::GLShaderObject *shader= gk::GLManager<gk::GLShaderObject>::manager().insert(
                        new gk::GLShaderObject(type));
                
                gk::TextFile *source= gk::TextFileIO::read(m_argv[i]);
                if(source == NULL)
                {
                    printf("error reading source '%s'.\n", m_argv[i]);
                    return -1;
                }
                else
                {
                    shader->pushSource(source);
                    program->attachShader(shader);
                }
            }
            
            code= program->createGLResource();
            program->releaseGLResource();
            delete program;
        }
        
        return code;
    }
    
    int quit( )
    {
        return 0;
    }
    
    int draw( )
    {
        // rien a faire
        return 0;
    }
};


int main( int argc, char *argv[] )
{
    if(argc < 2)
    {
        printf("usage: %s [-c] shader.*v* [shader.*c*]  [shader.*e*] [shader.*g*] shader.*f*\n", argv[0]);
        return 0;
    }

    GLSLCC app(argc, argv);
    int code= app.run();
    if(code < 0)
        printf("failed.\n");
    else
        printf("done.\n");
    return (code < 0) ? 1 : 0;
}
