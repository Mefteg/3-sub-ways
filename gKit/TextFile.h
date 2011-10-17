
#ifndef _TEXTFILE_H
#define _TEXTFILE_H

#include <cstdio>
#include <cstring>
#include <cassert>

#include <string>
#include <vector>
#include <map>

#include "IOResource.h"


namespace gk {

//! representation d'un vec4, valeur associee a une cle, cf. TextKey et TextFile.
struct Vec4Value
{
    float x, y, z, w;
    
    Vec4Value( const float _x= 0.f, const float _y= 0.f, const float _z= 0.f, const float _w= 0.f ) 
        :
        x(_x), y(_y), z(_z), w(_w)
    {}
    
    operator const float *( ) const
    {
        return &x;
    }
};

//! representation d'un vec3, valeur associee a une cle, cf. TextKey et TextFile.
struct Vec3Value
{
    float x, y, z;
    
    Vec3Value( const float _x= 0.f, const float _y= 0.f, const float _z= 0.f ) 
        :
        x(_x), y(_y), z(_z)
    {}
    
    operator const float *( ) const
    {
        return &x;
    }
};

//! representation d'un vec2, valeur associee a une cle, cf. TextKey et TextFile.
struct Vec2Value
{
    float x, y;
    
    Vec2Value( const float _x= 0.f, const float _y= 0.f) 
        :
        x(_x), y(_y)
    {}
    
    operator const float *( ) const
    {
        return &x;
    }
};

//! representation d'un float, valeur associee a une cle, cf. TextFile
struct FloatValue
{
    float x;
    
    FloatValue( const float v= 0.f)
        :
        x(v)
    {}
    
    operator float( )
    {
        return x;
    }
    
    operator const float *( ) const
    {
        return &x;
    }
};

//! representation d'un int, valeur associee a une cle, cf. TextFile.
struct IntValue
{
    int x;
    
    IntValue( const int v= 0)
        :
        x(v)
    {}
    
    operator int( )
    {
        return x;
    }
    
    operator const int *( ) const
    {
        return &x;
    }
};

//! representation d'une chaine de caracteres, valeur associee a une cle, cf. TextFile.
struct StringValue
{
    std::string string;
    
    StringValue( )
        :
        string()
    {}

    StringValue( const std::string& value )
        :
        string(value)
    {}
    
    operator const char *( ) const
    {
        return string.c_str();
    }
    
    operator const std::string&( ) const
    {
        return string;
    }
    
    const char *c_str( ) const
    {
        return string.c_str();
    }
};

//! representation d'un nom de fichier, valeur associee a une cle, cf. TextFile.
struct FileValue : public StringValue
{
    FileValue( )
        :
        StringValue()
    {}
    
    FileValue( const std::string& value )
        :
        StringValue(value)
    {}
};

//! representation d'un nom de variable, de section, etc. valeur associee a une cle, cf. TextFile.
struct NameValue : public StringValue
{
    NameValue( )
        :
        StringValue()
    {}
    
    NameValue( const std::string& value )
        :
        StringValue(value)
    {}
};

//! representation d'une valeur (non interpretee, texte) associee a une cle, cf. TextKey et TextFile.
class TextValue
{
    std::string value;

public:
    //! utilisation interne. constructeur.
    TextValue( );
    //! utilisation interne. constructeur.
    TextValue( const std::string& string );
    //! utilisation interne. constructeur.
    TextValue& operator= ( const TextValue& b );

    //! comparaison.
    bool operator== ( const TextValue& b ) const;
    //! comparaison.
    bool operator!= ( const TextValue& b ) const;
    
    //! destructeur.
    ~TextValue( ) {}
    
    //! interprete la valeur texte comme un vec4. syntaxe: vec4( x, y, z, w ).
    Vec4Value asVec4( ) const;
    //! interprete la valeur texte comme un vec3. syntaxe: vec3( x, y, z ).
    Vec3Value asVec3( ) const;
    //! interprete la valeur texte comme un vec2. syntaxe: vec2( x, y ).
    Vec2Value asVec2( ) const;
    //! interprete la valeur texte comme un float. syntaxe: float( v ).
    FloatValue asFloat( ) const;
    //! interprete la valeur texte comme un int. syntaxe: int( v ).
    IntValue asInt( ) const;
    //! renvoie la chaine de caracteres sans interpretation.
    StringValue asString( ) const;
    //! interprete la valeur texte comme un nom de fichier. syntaxe: "nom/de/fichier".
    FileValue asFile( ) const;
    //! renvoie un nom (de section, par exemple). syntaxe: identifiant_sans_espace.
    NameValue asName( ) const;

    //! affecte une nouvelle valeur.
    TextValue& operator= ( const Vec4Value& value );
    TextValue& operator= ( const Vec3Value& value );
    TextValue& operator= ( const Vec2Value& value );
    TextValue& operator= ( const FloatValue& value );
    TextValue& operator= ( const IntValue& value );
    TextValue& operator= ( const StringValue& value );
    TextValue& operator= ( const FileValue& value );
    TextValue& operator= ( const NameValue& value );
    
    //! utilisation interne. renvoie la chaine de caracteres representant la valeur.
    const char *c_str( ) const;
    
    //! renvoie une valeur non definie, permet de verifier le resultat d'une recherche de cle.
    static TextValue& notFound( )
    {
        static TextValue not_found("not_found");
        return not_found;
    }
};

//! representation d'une section d'un fichier texte / d'un fichier effet .gkfx.
struct TextSection
{
    std::string filename;
    int line;
    std::string text;
    
    TextSection( const std::string& _filename, const int _line= 1 )
        :
        filename(_filename),
        line(_line),
        text()
    {}
    
    TextSection( const std::string& _text, const std::string& _filename, const int _line )
        :
        filename(_filename),
        line(_line),
        text(_text)
    {}
};

//! representation d'un fichier effet (.gkfx), d'un texte, d'un source de shader, etc.
//! un TextFile est compose de plusieurs sections et peut etre interprete comme un ensemble de paires (cle, valeur) ou comme une seule valeur.
class TextFile : public IOResource
{
    std::vector<TextSection> m_definitions;
    std::vector<TextSection> m_sections;
    std::string m_name;
    
    std::map<std::string, TextValue> m_pairs;
    bool m_pairs_init;
    
    //! affecte le contenu du fichier a la section.
    int read( TextSection& section, FILE *in );
    
    //! renvoie l'ensemble de cles.
    std::map<std::string, TextValue>& pairs( );
    
public:
    TextFile( const std::string& name )
        :
        m_definitions(),
        m_sections(),
        m_name(name),
        m_pairs(),
        m_pairs_init(false)
    {}
    
    ~TextFile( ) {}
    
    //! lecture du fichier 'filename'.
    int read( const std::string& filename );
    
    //! inclure le contenu du fichier 'filename', a la suite du contenu deja charge.
    int include( const std::string& filename );

    //! insere le contenu de 'source', a la suite du contenu deja charge.
    int include( const std::string& source, const std::string& filename, const int line );

    //! insere le contenu de 'source', a la suite du contenu deja charge.
    int include( const TextFile *text );
    
    //! insere une definition dans l'entete du fichier, a la suite des autres definitions.
    int define( const std::string& what, const std::string& value );

    //! renvoie le contenu du fichier sous forme de chaine de caracteres std::string.
    std::string string( ) const;
    
    //! renvoie une ligne du texte et son 'origine' : nom du fichier et numero de ligne 
    int getLine( const int line, std::string &string, std::string& file_name, int &file_line ) const;
    
    //! renvoie le nom du fichier.
    const std::string& name( ) const
    {
        return m_name;
    }
    
    //! affiche le contenu du fichier.
    void print( ) const;
    
    //! interprete la premiere section comme un vec4. cf. TextValue pour la syntaxe a utiliser.
    Vec4Value asVec4( ) const;
    //! interprete la premiere section comme un vec3. cf. TextValue pour la syntaxe a utiliser.
    Vec3Value asVec3( ) const;
    //! interprete la premiere section comme un vec2. cf. TextValue pour la syntaxe a utiliser.
    Vec2Value asVec2( ) const;
    //! interprete la premiere section comme un reel. cf. TextValue pour la syntaxe a utiliser.
    FloatValue asFloat( ) const;
    //! interprete la premiere section comme un entier. cf. TextValue pour la syntaxe a utiliser.
    IntValue asInt( ) const;
    //! interprete la premiere section comme un nom (de section, par exemple). cf. TextValue pour la syntaxe a utiliser.
    NameValue asName( ) const;
    //! interprete la premiere section comme une chaine de caracteres. 
    StringValue asString( ) const;
    //! interprete la premiere section comme un nom de fichier. cf. TextValue pour la syntaxe a utiliser.
    FileValue asFile( ) const;
    
    //! utilisation interne. renvoie la chaine de caracteres de la premiere section.
    const char *c_str( ) const;

    //! recherche une cle et renvoie sa valeur. cf. TextValue pour obtenir un resultat type.
    //! renvoie TextValue::notFound() en cas d'echec / si la cle n'existe pas.
    TextValue& find( const std::string& key );

    //! ajoute ou modifie la valeur d'une cle.
    TextValue& insert( const std::string& key );
    
    //! enumerations des paires definies dans le TextFile.
    std::map<std::string, TextValue>::iterator begin( )
    {
        return pairs().begin();
    }
    
    //! enumerations des paires definies dans le TextFile.
    std::map<std::string, TextValue>::iterator end( )
    {
        return pairs().end();
    }
    
    //! renvoie une valeur non definie, permet de verifier le resultat d'une recherche de cle, cf find().
    static TextFile *notFound( )
    {
        static TextFile not_found("not_found");
        return &not_found;
    }    
};

}       // namespace

#endif
