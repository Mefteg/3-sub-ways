
#include <cstring>

#include "TextFile.h"


namespace gk {
    
int TextFile::read( TextSection& section, FILE *in )
{
    //! \todo re-utiliser la version de Effect
    char tmp[1024];
    for(;;)
    {
        if(fgets(tmp, sizeof(tmp), in) == NULL)
            break;
        
        // normalise les fins de lignes, \n pour windows, linux et mac os
        for(int i= 0; tmp[i] != 0; i++)
        {
            if(tmp[i] == '\r' && tmp[i+1] == '\n')
                // tmp[i+1] existe toujours, pas de reference apres la fin du tableau
                continue;
            
            if(tmp[i] == '\r')
                tmp[i]= '\n';
            section.text.push_back(tmp[i]);
        }
    }
    
    // force un saut de ligne a la fin du source
    section.text.push_back('\n');
    return 0;
}

int TextFile::read( const std::string& filename )
{
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
        return -1;
    
    m_sections.push_back( TextSection(filename) );
    int code= read(m_sections.back(), in);
    fclose(in);
    return code;
}

int TextFile::include( const std::string& filename )
{
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
        return -1;
    
    m_sections.push_back( TextSection(filename) );
    int code= read(m_sections.back(), in);
    fclose(in);
    return code;
}

//! insere le contenu de 'source', a la suite du contenu deja charge.
int TextFile::include( const std::string& source, const std::string& filename, const int line )
{
    m_sections.push_back( TextSection(source, filename, line) );
    return 0;
}

//! insere une definition dans l'entete du fichier, a la suite des autres definitions.
//! insere une ligne : #define what value
int TextFile::define( const std::string& what, const std::string& value )
{
    m_definitions.push_back( 
        TextSection("#define " + what + " " + value + "\n", "(definition)", 1) );
    return 0;
}

int TextFile::include( const TextFile *text )
{
    if(text == NULL)
        return -1;
    
    if(m_name.empty())
        m_name= text->m_name;
    
    m_definitions.insert(m_definitions.end(), 
        text->m_definitions.begin(), text->m_definitions.end());
    m_sections.insert(m_sections.end(),
        text->m_sections.begin(), text->m_sections.end());
    return 0;
}


//! renvoie le contenu du fichier sous forme de chaine de caracteres std::string.
std::string TextFile::string( ) const
{
    std::string text;
    
    int definitions_n= (int) m_definitions.size();
    for(int i= 0; i < definitions_n; i++)
        text.append(m_definitions[i].text);
    
    int sections_n= (int) m_sections.size();
    for(int i= 0; i< sections_n; i++)
        text.append(m_sections[i].text);
    
    return text;
}

void TextFile::print( ) const
{
    int definitions_n= (int) m_definitions.size();
    for(int i= 0; i < definitions_n; i++)
        printf("%s\n", m_definitions[i].text.c_str());
    
    int sections_n= (int) m_sections.size();
    for(int i= 0; i< sections_n; i++)
        printf("%s:%d\n%s\n", 
            m_sections[i].filename.c_str(), m_sections[i].line,
            m_sections[i].text.c_str());
}

int TextFile::getLine( const int line, std::string &string, std::string& file_name, int &file_line ) const
{
    file_name.clear();
    file_line= 0;
    
    if(line < 1)
        return -1;
    
    if(line < (int) m_definitions.size() +1)
    {
        string= m_definitions[line -1].text;
        file_name= m_definitions[line -1].filename;
        file_line= 1;
        return 0;
    }
    
    // parcourir les sections du texte
    int global_line= (int) m_definitions.size() +1;
    for(int i= 0; i < (int) m_sections.size(); i++)
    {
        const std::string& section= m_sections[i].text;

        int line_start= 0;
        int last_start= 0;
        int local_line= 1;
        for(int k= 0; k < (int) section.length(); k++)
        {
            line_start++;
            if(section[k] != '\n')
                continue;
            
            //~ printf("line %03d (%03d) %s", global_line, local_line, 
                //~ section.substr(last_start, line_start - last_start).c_str());
            if(line == global_line)
            {
                string= m_sections[i].text.substr(last_start, line_start - last_start);
                file_name= m_sections[i].filename;
                file_line= m_sections[i].line + local_line -1;
                return 0;
            }
            
            global_line++;
            local_line++;
            
            last_start= line_start;
        }
    }

    return -1;
}


TextValue::TextValue( )
    :
    value()
{}

TextValue::TextValue( const std::string& string )
    :
    value(string)
{}

TextValue& TextValue::operator= ( const TextValue& b )
{
    value= b.value;
    return *this;
}

bool TextValue::operator== ( const TextValue& b ) const
{
    return (value == b.value);
}

bool TextValue::operator!= ( const TextValue& b ) const
{
    return (value != b.value);
}

Vec4Value TextValue::asVec4( ) const
{
    Vec4Value v;
    if(sscanf(value.c_str(), 
        " vec4 ( %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f] ) ", &v.x, &v.y, &v.z, &v.w ) != 4)
        v= Vec4Value();
    return v;
}

Vec3Value TextValue::asVec3( ) const
{
    Vec3Value v;
    if(sscanf(value.c_str(), 
        " vec3 ( %f%*[ f,] %f%*[ f,] %f%*[ f] ) ", &v.x, &v.y, &v.z ) != 3)
        v= Vec3Value();
    return v;
}

Vec2Value TextValue::asVec2( ) const
{
    Vec2Value v;
    if(sscanf(value.c_str(), 
        " vec2 ( %f%*[ f,] %f%*[ f] ) ", &v.x, &v.y ) != 2)
        v= Vec2Value();
    return v;
}

FloatValue TextValue::asFloat( ) const
{
    FloatValue v;
    if(sscanf(value.c_str(), " float ( %f%*[ f] ) ", &v.x ) != 1)
        v= FloatValue();
    return v;
}

IntValue TextValue::asInt( ) const
{
    IntValue  v;
    if(sscanf(value.c_str(), " int ( %d ) ", &v.x ) != 1)
        v= IntValue();
    return v;
}

StringValue TextValue::asString( ) const
{
    return StringValue(value);
}

const char *TextValue::c_str( ) const
{
    return value.c_str();
}

FileValue TextValue::asFile( ) const
{
    char tmp[1024];
    if(sscanf(value.c_str(), " \"%1023[^\"]\" ", tmp) != 1)
        return FileValue();
    else
        return FileValue(tmp);
}

NameValue TextValue::asName( ) const
{
    char tmp[1024];
    if(sscanf(value.c_str(), " %s ", tmp) != 1)
        return NameValue();
    else
        return NameValue(tmp);
}

TextValue& TextValue::operator= ( const Vec4Value& v )
{
    char tmp[1024];
    sprintf(tmp, "vec4( %f, %f, %f, %f )", v.x, v.y, v.z, v.w);
    value.assign(tmp);
    return *this; 
}

TextValue& TextValue::operator= ( const Vec3Value& v )
{
    char tmp[1024];
    sprintf(tmp, "vec3( %f, %f, %f )", v.x, v.y, v.z);
    value.assign(tmp);
    return *this; 
}

TextValue& TextValue::operator= ( const Vec2Value& v )
{
    char tmp[1024];
    sprintf(tmp, "vec2( %f, %f )", v.x, v.y);
    value.assign(tmp);
    return *this; 
}

TextValue& TextValue::operator= ( const FloatValue& v )
{
    char tmp[1024];
    sprintf(tmp, "float( %f )", v.x);
    value.assign(tmp);
    return *this; 
}

TextValue& TextValue::operator= ( const IntValue& v )
{
    char tmp[1024];
    sprintf(tmp, "int( %d )", v.x);
    value.assign(tmp);
    return *this; 
}

TextValue& TextValue::operator= ( const StringValue& v )
{
    value.assign(v.c_str());
    return *this;
}

TextValue& TextValue::operator= ( const FileValue& v )
{
    char tmp[1024];
    sprintf(tmp, "\"%s\"", v.c_str());
    value.assign(tmp);
    return *this;
}

TextValue& TextValue::operator= ( const NameValue& v )
{
    char tmp[1024];
    if(sscanf(v.c_str(), " %s ", tmp) == 1)
        value.assign(tmp);
    
    return *this;
}


Vec4Value TextFile::asVec4( ) const
{
    if(m_sections.empty())
        return Vec4Value();
    return TextValue(m_sections[0].text).asVec4();
}

Vec3Value TextFile::asVec3( ) const
{
    if(m_sections.empty())
        return Vec3Value();
    return TextValue(m_sections[0].text).asVec3();
}

Vec2Value TextFile::asVec2( ) const
{
    if(m_sections.empty())
        return Vec2Value();
    return TextValue(m_sections[0].text).asVec2();
}

FloatValue TextFile::asFloat( ) const
{
    if(m_sections.empty())
        return FloatValue();
    return TextValue(m_sections[0].text).asFloat();
}

IntValue TextFile::asInt( ) const
{
    if(m_sections.empty())
        return IntValue();
    return TextValue(m_sections[0].text).asInt();
}

StringValue TextFile::asString( ) const
{
    if(m_sections.empty())
        return StringValue();
    return StringValue(m_sections[0].text);
}

const char *TextFile::c_str( ) const
{
    if(m_sections.empty())
        return "";
    return m_sections[0].text.c_str();
}

FileValue TextFile::asFile( ) const
{
    if(m_sections.empty())
        return FileValue();
    return TextValue(m_sections[0].text).asFile();
}

NameValue TextFile::asName( ) const
{
    if(m_sections.empty())
        return NameValue();
    return TextValue(m_sections[0].text).asName();
}

TextValue& TextFile::find( const std::string& key )
{
    std::map<std::string, TextValue>::iterator found= pairs().find(key);
    if(found == m_pairs.end())
        return TextValue::notFound();
    else
        return found->second;
}

TextValue& TextFile::insert( const std::string& key )
{
    return pairs().insert( std::make_pair(key, TextValue()) ).first->second;
}


std::map<std::string, TextValue>& TextFile::pairs( )
{
    if(m_pairs_init == true)
        return m_pairs;

    // construire l'ensemble de paires
    char tmp[1024];
    const int n= (int) m_sections.size();
    for(int i= 0; i < n; i++)
    {
        size_t end= std::string::npos;
        // . parcourir toutes les paires de la section
        for(size_t start= 0; start != std::string::npos; start= end)
        {
            // key = value \n key = ...
            // key = value \n

            int pos= 0;
            if(sscanf(m_sections[i].text.c_str() + start, " %[^ =] = %n", tmp, &pos) != 1)
                break;
            
            if(strcmp(tmp, "//") == 0)
            {
                // elimine les commentaires, determine le debut de la ligne suivante
                end= m_sections[i].text.find('\n', start + pos + 2);
            #ifdef VERBOSE
                printf("  skip line '%s'\n", m_sections[i].text.substr(start, end - start).c_str());
            #endif
                continue;
            }
            
            end= m_sections[i].text.find('=', start + pos);
            if(end != std::string::npos)
                end= m_sections[i].text.rfind('\n', end);

            std::string key(tmp);
            std::string value= m_sections[i].text.substr(start + pos, end - start - pos);
            
            //~ printf("  '%s'= '%s'\n", key.c_str(), value.c_str());
            if(m_pairs.insert( std::make_pair(key, TextValue(value)) ).second == false)
                printf("  key '%s' already defined.\n", key.c_str());
        }
    }
    
    m_pairs_init= true;
    return m_pairs;
}
    
}
