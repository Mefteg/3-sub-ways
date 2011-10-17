
#ifndef _IOMANAGER_H
#define _IOMANAGER_H

#include <string>
#include <map>

#ifdef VERBOSE_DEBUG
#include <cstdio>
#include <typeinfo>
#endif

namespace gk {

//! utilisation interne. nommage d'un objet importe.
struct IOName
{
    std::string m_filename;
    std::string m_name;
    
    IOName( const std::string& filename, const std::string& name )
        :
        m_filename(filename),
        m_name(name)
    {}
    
    ~IOName( ) {}
    
    const std::string& name( ) const
    {
        return m_name;
    }
    
    const std::string& filename( ) const
    {
        return m_filename;
    }
    
    //! necessaire pour l'insertion dans une std::map.
    bool operator<( const IOName& b ) const
    {
        if(m_filename < b.m_filename)
            return true;
        else if(m_filename > b.m_filename)
            return false;
        return (m_name < b.m_name);
    }
};

//! manager pour les 'objets' importes a partir d'un fichier.
template< class Object >
class IOManager
{
    // non copyable
    IOManager( const IOManager& );
    IOManager& operator=( const IOManager& );

protected:
    //! constructeur.
    IOManager( )
        :
        m_names_map(),
        m_objects_map()
    {}
    
    //! destructeur.
    virtual ~IOManager( )
    {
        for(typename names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
        {
            delete i->second;
        }
    }

    typedef std::map<IOName, Object *> names_map_type;
    names_map_type m_names_map;

    // stockage duplique des IONames, faire mieux... pour un vrai projet.
    typedef std::map<Object *, IOName> objects_map_type;
    objects_map_type m_objects_map;
    
public:
    //! reference un nouvel 'objet' par son nom et le nom du fichier d'import.
    Object *insert( Object *object, const std::string& filename, const std::string& name= "" )
    {
        if(object == NULL)
            return NULL;
        
    #ifdef VERBOSE_DEBUG
        printf("IOManager<%s> %p::insert( ): object %p '%s', filename '%s'\n", 
            typeid(Object).name(), this,
            object, name.c_str(), filename.c_str());
    #endif
        
        m_names_map.insert( std::make_pair( IOName(filename, name), object ) );
        m_objects_map.insert( std::make_pair( object, IOName(filename, name) ) );
        return object;
    }
    
    //! recherche un 'objet' deja importe.
    Object *find( const std::string& filename, const std::string& name= "" )
    {
        typename names_map_type::iterator found= m_names_map.find( IOName(filename, name) );
    #ifdef VERBOSE_DEBUG
        printf("IOManager<%s> %p::find( ): object '%s', filename '%s'... ", 
            typeid(Object).name(), this,
            name.c_str(), filename.c_str());
        if(found != m_names_map.end())
            printf("found object %p done.\n", found->second);
        else
            printf("failed.\n");
    #endif

        if(found == m_names_map.end())
            return NULL;
        else
            return found->second;
    }
    
    //! recherche le nom et le nom du fichier d'un 'objet' deja importe.
    const IOName *find( Object *object )
    {
        typename objects_map_type::iterator found= m_objects_map.find( object );
    #ifdef VERBOSE_DEBUG
        printf("IOManager<%s> %p::find( ): object %p... ", typeid(Object).name(), this, object);
        if(found != m_objects_map.end())
            printf("found object '%s', filename '%s' done.\n", 
                found->second.m_name.c_str(), found->second.m_filename.c_str());
        else
            printf("failed.\n");
    #endif
        
        if(found == m_objects_map.end())
            return NULL;
        else
            return &found->second;
    }
};

}       // namespace

#endif
