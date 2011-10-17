
#include "GLShaderObject.h"

namespace gk {

void GLShaderObject::printErrors( )
{
    GLint length= 0;
    glGetShaderiv(m_name, GL_INFO_LOG_LENGTH, &length);
    if(length == 0)
    {
        printf("GLShaderObject( ): error compiling shader (no info log).\n");
        return;
    }
    
    GLchar *log= new GLchar[length];
    glGetShaderInfoLog(m_name, (GLsizei) length, NULL, log);

#ifdef VERBOSE_DEBUG
    printf("GLShaderObject( ): compile errors:\n%s\n", log);
#endif
    
    // affiche la ligne du source + l'erreur associee
    int last_string= -1;
    int last_line= -1;
    std::string file_name;
    int file_line= -1;
    int code= -1;
    
    for(int i= 0; log[i] != 0; i++)
    {
        int string= 0, line= 0, position= 0;
        if(sscanf(&log[i], "%d ( %d ) : %n", &string, &line, &position) == 2        // nvidia syntax
        || sscanf(&log[i], "ERROR : %d : %d : %n", &string, &line, &position) == 2)  // ati syntax
        {
            if(string != last_string || line != last_line)
            {
                printf("\n");
                std::string source;
                code= m_text->getLine(line, source, file_name, file_line);
                if(code == 0)
                    printf("%s\n", source.c_str());
            }
        }
        else
            // afficher la ligne complete en cas d'erreur d'intrepretation du message d'erreur ...
            position= 0;
        
        // affiche la localisation de l'erreur 
        if(code == 0)
            printf("%s:%d: ", file_name.c_str(), file_line);
        
        // affiche l'erreur
        for(i+= position; log[i] != 0; i++)
        {
            printf("%c", log[i]);
            if(log[i] == '\n')
                break;
        }
        
        last_string= string;
        last_line= line;
    }
    
    delete [] log;
}

//! cree l'objet openGL.
int GLShaderObject::createGLResource( )
{
    if(m_name != 0)
        return 0;

    if(m_errors == true)
        return -1;
    
    if(m_text == NULL)
        return 0;
    
    const std::string& sources= m_text->string();
    if(sources.empty())
    {
        printf("GLShaderObject( ): error compiling shader (no sources !).\n");
        return -1;
    }
    
    m_name= glCreateShader(m_type);
    if(m_name == 0)
        return -1;
    
    const GLchar *sources_string= (const GLchar *) sources.c_str();
    glShaderSource(m_name, 1, &sources_string, NULL);
    glCompileShader(m_name);
    
    GLint code;
    glGetShaderiv(m_name, GL_COMPILE_STATUS,  &code);
    if(code == GL_FALSE)
    {
        printErrors();
        return -1;
    }
    
    return 0;
}
    
//! detruit l'objet openGL.
int GLShaderObject::releaseGLResource( )
{
    if(m_name == 0)
        return -1;
    
    glDeleteShader(m_name);
    m_name= 0;
    return 0;
}

}

