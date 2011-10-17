
#ifndef _GLSHADERIO_H
#define _GLSHADERIO_H

#include "IOManager.h"
#include "GL/GLShaderObject.h"

namespace gk {

class GLVertexShaderIO : public IOManager<GLVertexShader>
{
    // non copyable
    GLVertexShaderIO( const GLVertexShaderIO& );
    GLVertexShaderIO& operator=( const GLVertexShaderIO& );
    
    // private default constructor, singleton
    GLVertexShaderIO( )
        :
        IOManager<GLVertexShader>()
    {}
    
    ~GLVertexShaderIO( )
    {
        // detruire les ressources GL, ~IOManager() detruit les objets.
        for(names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
            i->second->releaseGLResource();
    }
    
public:
    //! importe le shader 'name' a partir du ficher 'filename'
    static
    GLVertexShader *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        GLVertexShader *vertex= manager().find(filename, name);
        if(vertex != NULL)
            return vertex;
        
        // cree le shader directement' sans utiliser GLManager / GLContext, l'objet ne doit etre gere que par un seul manager !!
        vertex= new GLVertexShader(filename);
        
        // reference le shader avec le manager
        return manager().insert(vertex, filename, name);
    }

    static
    GLVertexShaderIO& manager( )  // singleton
    {
        static GLVertexShaderIO manager;
        return manager;
    }
};


class GLFragmentShaderIO : public IOManager<GLFragmentShader>
{
    // non copyable
    GLFragmentShaderIO( const GLFragmentShaderIO& );
    GLFragmentShaderIO& operator&( const GLFragmentShaderIO& );
    
    // private default constructor, singleton
    GLFragmentShaderIO( )
        :
        IOManager<GLFragmentShader>()
    {}
    
    ~GLFragmentShaderIO( )
    {
        // detruire les ressources GL, ~IOManager() detruit les objets.
        for(names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
            i->second->releaseGLResource();
    }
    
public:
    //! importe le shader 'name' a partir du ficher 'filename'
    static
    GLFragmentShader *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        GLFragmentShader *fragment= manager().find(filename, name);
        if(fragment != NULL)
            return fragment;
        
        // cree le shader directement' sans utiliser GLManager / GLContext, l'objet ne doit etre gere que par un seul manager !!
        fragment= new GLFragmentShader(filename);
        
        // reference le shader avec le manager
        return manager().insert(fragment, filename, name);
    }

    static
    GLFragmentShaderIO& manager( )  // singleton
    {
        static GLFragmentShaderIO manager;
        return manager;
    }
};


#if defined GK_OPENGL3 || defined GK_OPENGL4
class GLGeometryShaderIO : public IOManager<GLGeometryShader>
{
    // non copyable
    GLGeometryShaderIO( const GLGeometryShaderIO& );
    GLGeometryShaderIO& operator&( const GLGeometryShaderIO& );
    
    // private default constructor, singleton
    GLGeometryShaderIO( )
        :
        IOManager<GLGeometryShader>()
    {}
    
    ~GLGeometryShaderIO( )
    {
        // detruire les ressources GL, ~IOManager() detruit les objets.
        for(names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
            i->second->releaseGLResource();
    }
    
public:
    //! importe le shader 'name' a partir du ficher 'filename'
    static
    GLGeometryShader *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        GLGeometryShader *geometry= manager().find(filename, name);
        if(geometry != NULL)
            return geometry;
        
        // cree le shader directement' sans utiliser GLManager / GLContext, l'objet ne doit etre gere que par un seul manager !!
        geometry= new GLGeometryShader(filename);
        
        // reference le shader avec le manager
        return manager().insert(geometry, filename, name);
    }

    static
    GLGeometryShaderIO& manager( )  // singleton
    {
        static GLGeometryShaderIO manager;
        return manager;
    }
};
#endif

#ifdef GK_OPENGL4
class GLControlShaderIO : public IOManager<GLControlShader>
{
    // non copyable
    GLControlShaderIO( const GLControlShaderIO& );
    GLControlShaderIO& operator&( const GLControlShaderIO& );
    
    // private default constructor, singleton
    GLControlShaderIO( )
        :
        IOManager<GLControlShader>()
    {}
    
    ~GLControlShaderIO( )
    {
        // detruire les ressources GL, ~IOManager() detruit les objets.
        for(names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
            i->second->releaseGLResource();
    }
    
public:
    //! importe le shader 'name' a partir du ficher 'filename'
    static
    GLControlShader *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        GLControlShader *control= manager().find(filename, name);
        if(control != NULL)
            return control;
        
        // cree le shader directement' sans utiliser GLManager / GLContext, l'objet ne doit etre gere que par un seul manager !!
        control= new GLControlShader(filename);
        
        // reference le shader avec le manager
        return manager().insert(control, filename, name);
    }

    static
    GLControlShaderIO& manager( )  // singleton
    {
        static GLControlShaderIO manager;
        return manager;
    }
};
#endif

#ifdef GK_OPENGL4
class GLEvaluationShaderIO : public IOManager<GLEvaluationShader>
{
    // non copyable
    GLEvaluationShaderIO( const GLEvaluationShaderIO& );
    GLEvaluationShaderIO& operator&( const GLEvaluationShaderIO& );
    
    // private default constructor, singleton
    GLEvaluationShaderIO( )
        :
        IOManager<GLEvaluationShader>()
    {}
    
    ~GLEvaluationShaderIO( )
    {
        // detruire les ressources GL, ~IOManager() detruit les objets.
        for(names_map_type::iterator 
            i= m_names_map.begin(); i != m_names_map.end(); ++i)
            i->second->releaseGLResource();
    }
    
public:
    //! importe le shader 'name' a partir du ficher 'filename'
    static
    GLEvaluationShader *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        GLEvaluationShader *evaluation= manager().find(filename, name);
        if(evaluation != NULL)
            return evaluation;
        
        // cree le shader directement' sans utiliser GLManager / GLContext, l'objet ne doit etre gere que par un seul manager !!
        evaluation= new GLEvaluationShader(filename);
        
        // reference le shader avec le manager
        return manager().insert(evaluation, filename, name);
    }

    static
    GLEvaluationShaderIO& manager( )  // singleton
    {
        static GLEvaluationShaderIO manager;
        return manager;
    }
};
#endif

}       // namespace

#endif

