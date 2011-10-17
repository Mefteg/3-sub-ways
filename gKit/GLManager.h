
#ifndef _GLMANAGER_H
#define _GLMANAGER_H

#include <string>
#include <map>
#include <vector>

namespace gk {

//! manager pour les ressources openGL crees directement par l'application.
template< class Object >
class GLManager
{
    typedef std::map<std::string, Object *> name_map_type;
    name_map_type m_names_map;
    
    std::vector<Object *> m_objects;

    //! non copyable.
    GLManager( const GLManager& );
    //! non copyable.
    GLManager& operator=( const GLManager& );
    
    //! constructeur.
    GLManager( )
        :
        m_names_map(),
        m_objects()
    {}
    
    //! destructeur.
    virtual ~GLManager( )
    {
        const int n= (int) m_objects.size();
        for(int i= 0; i < n; i++)
        {
            // libere les ressources openGL
            m_objects[i]->releaseGLResource();
            // et detruit l'objet
            delete m_objects[i];
        }
    }
    
public:
    //! reference un nouvel objet nomme.
    Object *insert( Object *object, const std::string& name= "" )
    {
        if(name.empty() == false)
            // n'insere que les objets nommes
            m_names_map.insert( std::make_pair(name, object) );
        
        m_objects.push_back(object);
        return object;
    }
    
    //! recherche un objet nomme.
    Object *find( const std::string& name )
    {
        typename name_map_type::iterator found= m_names_map.find(name);
        if(found == m_names_map.end())
            return NULL;
        else
            return found->second;
    }
    
    //! detruit un objet nomme.
    int release( const std::string& name )
    {
        typename name_map_type::iterator found= m_names_map.find(name);
        if(found == m_names_map.end())
            return -1;

        // libere les ressources openGL, de-reference l'objet
        found->second->releaseGLResource();
        delete found->second;
        m_names_map.erase(found);
        return 0;
    }
    
    static
    GLManager& manager( )
    {
        static GLManager manager;
        return manager;
    }
};

}       // namespace

#endif
