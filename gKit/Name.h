
#ifndef _GK_NAME_H
#define _GK_NAME_H

#include <string>
#include <cstring>
#include <cassert>


namespace gk {

#define GK_NAME_MAX 40
#define GK_NAME_HASH_SEED 1

//! representation d'un identifiant par une chaine de caracteres et / ou un index.
//! longueur limitee a 40 caracteres.
//! les comparaisons / recherches sur la chaine de caracteres sont remplacees par une comparaison sur une valeur de hachage.
class Name
{
#ifndef GK_NAME_NOSTRING
    char m_name[GK_NAME_MAX]; //!< chaine de caracteres.
#endif
    
    unsigned int m_hash;        //!< hachage de la chaine de caracteres. 
    int m_id;   //!< index / identifiant numerique 
    
    //! calcul de la valeur de hacahge par l'algorithme murmurhash2 cf http://sites.google.com/site/murmurhash/
    static
    unsigned int murmur2( const char *string, const int length, const unsigned int seed )
    {
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        // Initialize the hash to a 'random' value
        unsigned int h = seed ^ length;
        int len= length;

        // Mix 4 bytes at a time into the hash
        const unsigned char *data = (const unsigned char *) string;
        while(len >= 4)
        {
            unsigned int k = * (const unsigned int *) data;

            k *= m; 
            k ^= k >> r; 
            k *= m; 

            h *= m; 
            h ^= k;

            data += 4;
            len -= 4;
        }

        // Handle the last few bytes of the input array
        switch(len)
        {
            case 3: h ^= data[2] << 16;
            case 2: h ^= data[1] << 8;
            case 1: h ^= data[0];
            h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    //! utilisation interne.
    void hash( const char *string )
    {
        m_hash= murmur2(string, GK_NAME_MAX, GK_NAME_HASH_SEED);
    }
    
public:
    //! constructeur par defaut.
    Name( ) 
        :
        m_hash(0),
        m_id(-1)
    {
    #ifndef GK_NAME_NOSTRING
        m_name[0]= 0;
    #endif
    }
    
    //! constructeur. identifiant numerique.
    Name( const int id )
        :
        m_hash(0),
        m_id(id)
    {
    #ifndef GK_NAME_NOSTRING
        m_name[0]= 0;
    #endif
    }
    
    //! constructeur. chaine de caracteres.
    Name( const char *string )
        :
        m_hash(0),
        m_id(-1)
    {
    #ifndef GK_NAME_NOSTRING
        strncpy(m_name, string, GK_NAME_MAX);
        m_name[GK_NAME_MAX -1]= 0;
        hash(m_name);

    #else
        char name[GK_NAME_MAX];
        strncpy(name, string, GK_NAME_MAX);
        name[GK_NAME_MAX -1]= 0;
        hash(name);
    #endif
    }
    
    //! constructeur. chaine de caracteres c++.
    Name( const std::string& string )
        :
        m_hash(0),
        m_id(-1)
    {
    #ifndef GK_NAME_NOSTRING
        strncpy(m_name, string.c_str(), GK_NAME_MAX);
        m_name[GK_NAME_MAX -1]= 0;
        hash(m_name);

    #else
        char name[GK_NAME_MAX];
        strncpy(name, string.c_str(), GK_NAME_MAX);
        name[GK_NAME_MAX -1]= 0;
        hash(name);
    #endif
    }

    //! constructeur complet (chaine + id).
    Name( const char *string, const int id )
        :
        m_hash(0),
        m_id(id)
    {
    #ifndef GK_NAME_NOSTRING
        strncpy(m_name, string, GK_NAME_MAX);
        m_name[GK_NAME_MAX -1]= 0;
        hash(m_name);

    #else
        char name[GK_NAME_MAX];
        strncpy(name, string, GK_NAME_MAX);
        name[GK_NAME_MAX -1]= 0;
        hash(name);
    #endif
    }
    
    //! constructeur.  (chaine  c++ + id).
    Name( const std::string& string, const int id )
        :
        m_hash(0),
        m_id(id)
    {
    #ifndef GK_NAME_NOSTRING
        strncpy(m_name, string.c_str(), GK_NAME_MAX);
        m_name[GK_NAME_MAX -1]= 0;
        hash(m_name);

    #else
        char name[GK_NAME_MAX];
        strncpy(name, string.c_str(), GK_NAME_MAX);
        name[GK_NAME_MAX -1]= 0;
        hash(name);
    #endif
    }
    
    //! constructeur.
    Name( const Name& b )
    {
    #ifndef GK_NAME_NOSTRING
        strcpy(m_name, b.m_name);
    #endif
        m_hash= b.m_hash;
        m_id= b.m_id;
    }
    
    //! affectation.
    Name& operator=( const Name&b )
    {
    #ifndef GK_NAME_NOSTRING
        strcpy(m_name, b.m_name);
    #endif
        m_hash= b.m_hash;
        m_id= b.m_id;
        return *this;
    }
    
    //! renvoie la valeur de hachage.
    unsigned int hash( ) const
    {
        return m_hash;
    }
    
    //! renvoie l'identifiant numerique.
    unsigned int id( ) const
    {
        return m_id;
    }
    
    //! renvoie la chaine de caracteres, si definie.
    const char *c_str( ) const
    {
    #ifndef GK_NAME_NOSTRING
        return m_name;
    #else
        return "(Name NOSTRING)";
    #endif
    }
    
    //! renvoie la chaine de caracteres, si definie.
    const char *c_str( )
    {
    #ifndef GK_NAME_NOSTRING
        return m_name;
    #else
        return "(Name NOSTRING)";
    #endif
    }
    
    //! comparaisons.
    friend bool operator==( const Name& a, const Name& b );
    //! comparaisons.
    friend bool operator!=( const Name& a, const Name& b );
    //! comparaisons.
    friend bool operator<( const Name& a, const Name& b );
    //! comparaisons.
    friend bool operator>( const Name& a, const Name& b );
    //! comparaisons.
    friend int compare( const Name& a, const Name& b );
    
    //! calcule la valeur de hachage d'une chaine de caractere quelconque.
    static
    unsigned int getHash( const char * string )
    {
        return murmur2(string, strlen(string), GK_NAME_HASH_SEED);
    }

    //! calcule la valeur de hachage d'une chaine de caractere quelconque.
    static
    unsigned int getHash( const std::string& string )
    {
        return murmur2(string.c_str(), string.size(), GK_NAME_HASH_SEED);
    }
    
    //! \todo construction incrementale a partir de plusieurs chaines / cles intermediaires, pour la construction des identifiants mesh, mesh_material, shader, etc ?
    //! cf murmurhash2a.cpp et ecrire les operateurs + << = += ? au lieu des fonctions utilisees dans l'exemple.
};

inline
bool operator==( const Name& a, const Name& b )
{
    if(a.m_id < 0 || b.m_id < 0)
    {
    #ifdef GK_NAME_NOSTRING
        return (a.m_hash == b.m_hash);
    #else
        return (a.m_hash == b.m_hash) && (strcmp(a.m_name, b.m_name) == 0);
    #endif
    }
    else
    {
        return (a.m_id == b.m_id);
    }
}

inline
bool operator!=( const Name& a, const Name& b )
{
    if(a.m_id < 0 || b.m_id < 0)
        return (a.m_hash != b.m_hash);
    else
        return (a.m_id != b.m_id);
}

inline
bool operator>( const Name& a, const Name& b )
{
    if(a.m_id < 0 || b.m_id < 0)
        return (a.m_hash > b.m_hash);
    else
        return (a.m_id > b.m_id);
}

inline
bool operator<( const Name& a, const Name& b )
{
    if(a.m_id < 0 || b.m_id < 0)
        return (a.m_hash < b.m_hash);
    else
        return (a.m_id < b.m_id);}

inline
int compare( const Name& a, const Name& b )
{
    if(a.m_id < 0 || b.m_id < 0)
    {
        if(a.m_hash < b.m_hash)
            return -1;
        else if(a.m_hash > b.m_hash)
            return 1;
        else
    #ifdef GK_NAME_NOSTRING
            return 0;
    #else
            return strcmp(a.m_name, b.m_name);
    #endif
    }
    else
    {
        if(a.m_id < b.m_id)
            return -1;
        else if(a.m_id > b.m_id)
            return 1;
        else
            return 0;
    }
}

}

#endif

