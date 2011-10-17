
#ifndef _MESH_MATERIAL_H
#define _MESH_MATERIAL_H

#include <string>

#include "IOResource.h"
#include "Geometry.h"

namespace gk {

//! representation d'une matiere.
struct MeshMaterial : public IOResource
{
    float kd; //!< diffus
    float ks; //!< reflechissant
    float n;       //!< puissance du reflet
    float ni; //!< indice de refraction
    
    Energy diffuse;    //!< couleur diffuse 
    Energy specular;   //!< couleur du reflet
    Energy transmission;   //!< couleur du 'reflet' refracte
    Energy emission;    //!< flux emis par la surface, en watt / m**2
    
    std::string diffuse_texture;      //!< nom de la texture.
    std::string glossy_texture;       //!< nom de la texture.
    std::string name;         //!< nom de la matiere.
    
    //! construction d'une matiere nommee.
    MeshMaterial( const std::string& _name )
        :
        kd(.5f), 
        ks(0.f), n(1.f), 
        ni(1.f),
        diffuse(1.f, 1.f, 1.f),
        specular(1.f, 1.f, 1.f),
        transmission(),
        emission(),
        diffuse_texture(),
        glossy_texture(),
        name(_name)
    {}
    
    //! destructeur.
    ~MeshMaterial( ) {}
};

}       // namespace

#endif

