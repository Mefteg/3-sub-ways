
#ifndef _IOMESH_H
#define _IOMESH_H

#include "IOManager.h"

#include "Mesh.h"
#include "MeshOBJ.h"

//~ #include "MeshGK.h"
#define NO_MESHGK

namespace gk {

//! manager pour importer des maillages.
class MeshIO : public IOManager<Mesh>
{
    // non copyable
    MeshIO( const MeshIO& );
    MeshIO& operator=( const MeshIO& );
    
    // private default constructor, singleton
    MeshIO( )
        :
        IOManager<Mesh>()
    {}
    
    ~MeshIO( ) {}
    
public:
    //! importe l'objet 'name' du fichier 'filename'
    static
    Mesh *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        Mesh *mesh= manager().find(filename, name);
        if(mesh != NULL)
            return mesh;
        
        // importer le fichier
        mesh= new Mesh;
        if(isMeshOBJ(filename) && MeshLoadFromOBJ(filename, mesh) < 0)
        {
            printf("'%s' failed.\n", filename.c_str());
            delete mesh;
            return NULL;
        }
        
    #ifndef NO_MESHGK
        if(isMeshGK(filename) && MeshLoadFromGK(filename, mesh) < 0)
        {
            printf("'%s' failed.\n", filename.c_str());
            delete mesh;
            return NULL;
        }
    #endif
        
        // reference le mesh avec le manager
        return manager().insert(mesh, filename, name);
    }

#ifndef NO_MESHGK
    //! ecrit un mesh dans le fichier nomme 'filename'. 
    //! format .gkmesh uniquement, pour l'instant.
    static
    int write( const Mesh *mesh, const std::string& filename )
    {
        if(isMeshGK(filename) && MeshWriteToGK(mesh, filename) < 0)
            return -1;
        return 0;
    }
#endif
    
    static
    MeshIO& manager( )  // singleton
    {
        static MeshIO manager;
        return manager;
    }
};

} // namespace

#endif
