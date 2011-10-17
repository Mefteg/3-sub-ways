
#ifndef _MESHOBJ_H
#define _MESHOBJ_H

#include <string>

namespace gk {

class Mesh;
//! renvoie vrai si 'filename' se termine par '.obj'.
bool isMeshOBJ( const std::string& filename );

int MeshLoadFromOBJ( const std::string& filename, Mesh *mesh );
int MaterialLoadFromMTL( const std::string& filename );

//! namespace prive.
/*! namespace prive, pour isoler les classes utilisees pour charger les objets maya .obj et leurs matieres .mtl.
    
    chargement d'un objet maya obj + construction lineaire d'un maillage triangule.

    cf http://local.wasp.uwa.edu.au/~pbourke/dataformats/obj/
    et  http://local.wasp.uwa.edu.au/~pbourke/dataformats/mtl/
 */    
namespace OBJ {}    
    
}

#endif
