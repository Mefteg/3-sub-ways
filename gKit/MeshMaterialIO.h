
#ifndef _IOMESH_MATERIAL_H
#define _IOMESH_MATERIAL_H

#include <cstdio>

#include "IOManager.h"
#include "MeshMaterial.h"
#include "MeshOBJ.h"

namespace gk {

//! manager pour importer des matieres.
class MeshMaterialIO : public IOManager<MeshMaterial>
{
    // non copyable
    MeshMaterialIO( const MeshMaterialIO& );
    MeshMaterialIO& operator=( const MeshMaterialIO& );
    
    // private default constructor, singleton
    MeshMaterialIO( )
        :
        IOManager<MeshMaterial>()
    {}
    
    ~MeshMaterialIO( ) {}
    
public:
    //! importe la matiere 'name' du fichier 'filename'
    static
    MeshMaterial *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        MeshMaterial *material= manager().find(filename, name);
        if(material != NULL)
            return material;
        
        // importer le fichier et referencer toutes les matieres dans le manager
        if(MaterialLoadFromMTL(filename) < 0)
        {
            printf("'%s' failed.\n", filename.c_str());
            return NULL;
        }
        
        // rechercher et renvoyer la matiere 'name'
        return manager().find(filename, name);
    }

    static
    MeshMaterialIO &manager( )  // singleton
    {
        static MeshMaterialIO manager;
        return manager;
    }
};

}       // namespace

#endif
