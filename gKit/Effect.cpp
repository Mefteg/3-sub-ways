
#include <cstdio>

#include "Effect.h"


namespace gk {

Effect::Effect( )
    :
    IOResource(),
    m_text(),
    m_last_line(),
    m_last_section(),
    m_line_number(1),
    m_last_char(0)
{}
    

Effect::Effect( const std::string& filename )
    :
    IOResource(),
    m_text(),
    m_last_line(),
    m_last_section(),
    m_line_number(1),
    m_last_char(0)
{
    read(filename);
}

Effect::~Effect( ) {}

int Effect::read_line( FILE *in )
{
    m_last_line.clear();
    for(;;)
    {
        int c= fgetc(in);
        if(c == EOF)
            return -1;
        
        if((c == '\r')    // windows + mac
        || (c == '\n' && m_last_char != '\r'))        // linux
        {
            m_line_number++;
            m_last_line.push_back('\n');        // fin de ligne normalisee
            m_last_char= c;
            return 0;
        }
        
        if(c != '\n')
        {
            m_last_line.push_back(c);
            m_last_char= c;
        }
    }
}

int Effect::read_section( FILE *in )
{
    m_last_section.clear();
    char tmp[1024];
    for(;;)
    {
        if(read_line(in) < 0)
            return -1;
        
        // detecter la fin de section
        if(sscanf(m_last_line.c_str(), " -- %1023[^ \n] \n ", tmp) == 1)
            return 0;
        m_last_section.append(m_last_line);
    }
    
    return 0;
}

int Effect::read( const std::string& filename )
{
#ifdef VERBOSE
    printf("loading effect file '%s'...\n", filename.c_str());
#endif

    FILE *in= fopen(filename.c_str(), "rb");
    if(in == NULL)
    {
    #ifdef VERBOSE
        printf("failed.\n");
    #endif
        return -1;
    }
    
    // lire l'entete
    if(read_section(in) < 0)
    {
    #ifdef VERBOSE
        printf("failed.\n");
    #endif
        fclose(in);
        return -1;
    }
    
    // lire les sections
    char key[1024];
    int section_begin;
    for(;;)
    {
        if(sscanf(m_last_line.c_str(), " -- %1023[^ \n] \n ", key) != 1)
            break;
        section_begin= m_line_number;
        read_section(in);
    #ifdef VERBOSE
        printf("  key '%s', line %d\n", key, section_begin);
    #endif
        
        // . conserver la section 
        m_text.push_back( new TextFile(key) );
        m_text.back()->include(m_last_section, filename, section_begin);
    }
    
#ifdef VERBOSE
    printf("done.\n");
#endif
    fclose(in);
    return 0;
}

TextFile *Effect::find( const std::string& key ) const
{
    // debile et sans surpise ...
    const int n= (int) m_text.size();
    for(int i= 0; i < n; i++)
        if(m_text[i]->name() == key)
            return m_text[i];
        
#ifdef VERBOSE
    printf("Effect::find('%s'): not found, using default value.\n", key.c_str());
#endif
    
    return TextFile::notFound();
}


}
