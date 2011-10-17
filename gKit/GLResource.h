
#ifndef _GLRESOURCE_H
#define _GLRESOURCE_H


namespace gk {

//! classe de base des resources openGL, referencees par l'application, proprietes du contexte openGL. cf \ref GLResource, gestion des objets openGL.
class GLResource
{
    //! non copyable.
    GLResource( const GLResource& );
    //! non copyable.
    GLResource& operator=( const GLResource& );
    
public:
    GLResource( ) {}
    virtual ~GLResource( ) {}
    
    //! comportement par defaut, necessaire pour GLManager, a redefinir dans les classes derivees.
    int createGLResource( ) 
    {
        return 0;
    }
    
    //! comportement par defaut, necessaire pour GLManager, a redefinir dans les classes derivees.
    int releaseGLResource( )
    {
        return 0;
    }
};

} // namespace

#endif
