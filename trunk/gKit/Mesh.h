/* version allegee ?? mailto:jean-claude.iehl@liris.cnrs.fr */

#ifndef _MESH_H
#define _MESH_H

#include <cstring>
#include <vector>
#include <map>
#include <cassert>

#include "IOResource.h"
#include "MeshMaterial.h"
#include "Geometry.h"
#include "Triangle.h"
#include "PNTriangle.h"
#include "Name.h"


namespace gk {

//! representation d'une partie d'un maillage associee a une matiere unique identifiee par son indice 'material_id'.
struct SubMesh
{
    int begin;        //!< premier vertex du submesh.
    int end;  //!< dernier vertex du submesh.
    int material_id;  //!< indice de la matiere associee au submesh, cf. gk::Mesh::m_materials.
    
    //! constructeur.
    SubMesh( )
        :
        begin(0),
        end(0),
        material_id(0)
    {}
    
    //! constructeur par defaut.
    SubMesh( const int _begin, const int _end, const int _material_id )
        :
        begin(_begin),
        end(_end),
        material_id(_material_id)
    {}

    //! destructeur.
    ~SubMesh( ) {}
};

//! representation d'un ensemble d'attributs generiques des sommets du maillage. equivalent a un GLBuffer.
struct MeshBuffer
{
    Name semantic;      //!< nom de l'attribut
    
    int count;                  //!< nombre de vecteurs
    int size;       //!< 1, 2, 3, 4, dimension des vecteurs
    
    std::vector<float> data;    //!< stockage des attributs

    //! constructeur par defaut.
    MeshBuffer( )
        :
        count(0), size(0)
    {}

    //! constructeur.
    MeshBuffer( const Name& _semantic, const int _size )
        :
        semantic(_semantic),
        count(0), size(_size)
    {}
    
    //! insertion d'un attribut generique.
    void push( const float *attribute )
    {
        count++;
        
        for(int i= 0; i < size; i++)
            data.push_back(attribute[i]);
    }
    
    //! insertion d'un attribut point 2d.
    int push( const Point2& attribute )
    {
        if(size != 2)
            return -1;
        
        push(&attribute[0]);
        return 0;
    }
    
    //! insertion d'un attribut point 3d.
    int push( const Point& attribute )
    {
        if(size != 3)
            return -1;
        
        push(&attribute[0]);
        return 0;
    }
    
    //! insertion d'un attribut vecteur 3d.
    int push( const Vector& attribute )
    {
        if(size != 3)
            return -1;
        
        push(&attribute[0]);
        return 0;
    }

    //! insertion d'un attribut normale 3d.
    int push( const Normal& attribute )
    {
        if(size != 3)
            return -1;
        
        push(&attribute[0]);
        return 0;
    }
    
    //! insertion d'un attribut couleur rgba.
    int push( const Color& attribute )
    {
        if(size != 4)
            return -1;
        
        push(&attribute[0]);
        return 0;
    }

    //! renvoie le nombre d'attributs stockes.
    int attributeCount( ) const
    {
        return count;
    }
};


class MeshIO;

//! representation d'un maillage triangule.

//! un mesh est un ensemble de triangles.
//! un triangle est un triplet <abc> d'indices permettant de retrouver la position, la normale et les coordonnees de texture de chaque sommet a, b, c.
//! un triangle (cf. Material) a egalement une matiere associee.
class Mesh : public IOResource
{
    friend class MeshIO;
    
    std::vector<Point> m_positions;
    std::vector<Normal> m_normals;
    std::vector<Point2> m_texcoords;
    
    std::vector<int>m_indices;  //!< 3*triangles.size()
    std::vector<int> m_materials_id;     //!< triangles.size()

    std::vector<int> m_smooth_groups;   //!< triangles.size()

    std::vector<int> m_position_adjacency;        //!< positions.size(), premier element de la liste d'adjacence du sommet.
    std::vector<int> m_adjacency;       //!< 2* positions.size() ? liste globale m_adjacency[m_position_adjacency[id]] .. -1

    std::vector<SubMesh> m_submeshes;
    
    std::vector<MeshMaterial *> m_materials;
    MeshMaterial m_default_material;

    std::vector<MeshBuffer *> m_attributes_buffer;

    BBox m_bbox;

    //! renvoie la position d'un sommet.
    Point& position( const int id )
    {
        assert(id >= 0 && id < (int) m_positions.size());
        return m_positions[id];
    }
    
    //! renvoie la position d'un sommet.
    const Point& position( const int id ) const
    {
        assert(id >= 0 && id < (int) m_positions.size());
        return m_positions[id];
    }
    
    //! renvoie la normale d'un sommet.
    Normal& normal( const int id )
    {
        return m_normals[id];
    }
    
    //! renvoie la normale d'un sommet.
    const Normal& normal( const int id ) const
    {
        return m_normals[id];
    }
    
    //! renvoie la coordonnee de texture d'un sommet.
    Point2& texcoords( const int id )
    {
        return m_texcoords[id];
    }
    
    //! renvoie la coordonnees de texture d'un sommet.
    const Point2& texcoords( const int id ) const
    {
        return m_texcoords[id];
    }
    
    //! renvoie une matiere.
    MeshMaterial& material( const int id )
    {
        return *m_materials[id];
    }
    
    //! renvoie une matiere.
    const MeshMaterial& material( const int id ) const
    {
        return *m_materials[id];
    }
    
public:
    //! constructeur par defaut.
    Mesh( ) 
        :
        m_default_material("default")
    {}
    
    //! destructeur.
    ~Mesh( )
    {
        const int n= (int) m_attributes_buffer.size();
        for(int i= 0; i < n; i++)
            delete m_attributes_buffer[i];
    }
    
    //! ajoute un sommet.
    void pushPosition( const Point& point )
    {
        m_positions.push_back( point );
        m_bbox.Union(point);
    }
    
    //! ajoute un ensemble de sommets.
    void attachPositionBuffer( const std::vector<Point>& positions )
    {
        m_positions= positions;
    }

    //! ajoute un ensemble de sommets.
    void attachPositionBuffer( const int n, const Point *positions )
    {
        m_positions= std::vector<Point>(&positions[0], &positions[n]);
    }

    //! renvoie le nombre de sommets du maillage.
    int positionCount( ) const
    {
        return (int) m_positions.size();
    }
    
    //! renvoie les positions des sommets du maillage. 
    //~ const std::vector<Point>& positions( )
    std::vector<Point>& positions( )
    {
        return m_positions;
    }
    
    //! renvoie les positions des sommets du maillage. 
    const std::vector<Point>& positions( ) const
    {
        return m_positions;
    }
    
    //! ajoute une normale.
    void pushNormal( const Normal& normal )
    {
        m_normals.push_back( normal );
    }
    
    //! ajoute un ensemble de normales
    void attachNormalBuffer( const std::vector<Normal>& normals )
    {
        m_normals= normals;
    }
        
    //! ajoute un ensemble de normales
    void attachNormalBuffer( const int n, const Normal *normals )
    {
        m_normals= std::vector<Normal>(&normals[0], &normals[n]);
    }

    //! renvoie le nombre de normales du maillage.
    int normalCount( ) const
    {
        return (int) m_normals.size();
    }
    
    //! renvoie les normales.
    //~ const std::vector<Normal>& normals( )
    std::vector<Normal>& normals( )
    {
        return m_normals;
    }
    
    //! renvoie les normales.
    const std::vector<Normal>& normals( ) const
    {
        return m_normals;
    }

    //! ajoute une coordonnee de texture.
    void pushTexCoord( const Point2& texcoord )
    {
        m_texcoords.push_back( texcoord );
    }
    
    //! ajoute un ensemble de coordonnees de texture.
    void attachTexCoordBuffer( const std::vector<Point2>& texcoords )
    {
        m_texcoords= texcoords;
    }
    
    //! ajoute un ensemble de coordonnees de texture.
    void attachTexCoordBuffer( const int n, const Point2 *texcoords )
    {
        m_texcoords= std::vector<Point2>(&texcoords[0], &texcoords[n]);
    }

    //! renvoie le nombre de coordonnees de textures des sommets du maillage.
    int texCoordCount( ) const
    {
        return (int) m_texcoords.size();
    }
    
    //! renvoie les coordonnees de textures des sommets du maillage.
    //~ const std::vector<Point2>& texCoords( )
    std::vector<Point2>& texCoords( )
    {
        return m_texcoords;
    }
    
    //! renvoie les coordonnees de textures des sommets du maillage.
    const std::vector<Point2>& texCoords( ) const
    {
        return m_texcoords;
    }
    
    //! renvoie les groupes de lissage de normales du maillage.
    const std::vector<int>& smoothGroups( )
    {
        return m_smooth_groups;
    }
    
    //! renvoie les groupes de lissage de normales du maillage.
    const std::vector<int>& smoothGroups( ) const
    {
        return m_smooth_groups;
    }
    
    //! attache un ensemble d'attributs.
    template< class T >
    int attachAttributeBuffer( const Name& semantic, const std::vector<T>& attributes )
    {
        MeshBuffer *buffer= attachAttributeBuffer(semantic, T());
        if(buffer == NULL)
            return -1;
        
        const int n= (int) attributes.size();
        for(int i= 0; i < n; i++)
            buffer->push(attributes[i]);
        return 0;
    }
    
    //! attache un ensemble d'attributs.
    template< class T >
    int attachAttributeBuffer( const Name& semantic, const int n, const T *attributes )
    {
        MeshBuffer *buffer= attachAttributeBuffer(semantic, T());
        if(buffer == NULL)
            return -1;
        
        for(int i= 0; i < n; i++)
            buffer->push(attributes[i]);
        return 0;
    }

    //! declare un ensemble d'attributs point 2d.
    MeshBuffer *attachAttributeBuffer( const Name& semantic, const Point2& attribute_tag )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer != NULL)
            return NULL;        // deja attache
        
        buffer= new MeshBuffer(semantic, 2);
        m_attributes_buffer.push_back(buffer);
        return buffer;
    }
    
    //! declare un ensemble d'attributs point 3d.
    MeshBuffer *attachAttributeBuffer( const Name& semantic, const Point& attribute_tag )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer != NULL)
            return NULL;        // deja attache
        
        buffer= new MeshBuffer(semantic, 3);
        m_attributes_buffer.push_back(buffer);
        return buffer;
    }
    
    //! declare un ensemble d'attributs vecteur 3d.
    MeshBuffer *attachAttributeBuffer( const Name& semantic, const Vector& attribute_tag )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer != NULL)
            return NULL;        // deja attache
        
        buffer= new MeshBuffer(semantic, 3);
        m_attributes_buffer.push_back(buffer);
        return buffer;
    }
    
    //! declare un ensemble d'attributs normale 3d.
    MeshBuffer *attachAttributeBuffer( const Name& semantic, const Normal& attribute_tag )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer != NULL)
            return NULL;        // deja attache
        
        buffer= new MeshBuffer(semantic, 3);
        m_attributes_buffer.push_back(buffer);
        return buffer;
    }
    
    //! declare un ensemble d'attributs couleur rgba.
    MeshBuffer *attachAttributeBuffer( const Name& semantic, const Color& attribute_tag )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer != NULL)
            return NULL;        // deja attache
        
        buffer= new MeshBuffer(semantic, 4);
        m_attributes_buffer.push_back(buffer);
        return buffer;
    }

    //! renvoie un buffer d'apres son nom / semantique.
    MeshBuffer *findBuffer( const Name& semantic )
    {
        const int n= (int) m_attributes_buffer.size();
        for(int i= 0; i < n; i++)
            if(m_attributes_buffer[i]->semantic == semantic)
                return m_attributes_buffer[i];
        
        return NULL;
    }
    
    //! renvoie le nombre de buffers d'attributs.
    int bufferCount( ) const
    {
        return (int) m_attributes_buffer.size();
    }
    
    //! renvoie un buffer.
    //~ const MeshBuffer *buffer( const int id ) const
    MeshBuffer *buffer( const int id ) 
    {
        return m_attributes_buffer[id];
    }
    
    //! renvoie un buffer.
    const MeshBuffer *buffer( const int id ) const
    {
        return m_attributes_buffer[id];
    }
    
    //! insere un attribut dans l'ensemble associe.
    template < class T >
    void pushAttribute( const Name& semantic, const T& attribute )
    {
        MeshBuffer *buffer= findBuffer(semantic);
        if(buffer == NULL)
            return;
        buffer->push(attribute);
    }
    
    
    //! ajoute un triangle
    void pushTriangle( const int a, const int b, const int c, const int material_id, const int smooth_group= -1 )
    {
        m_indices.push_back(a);
        m_indices.push_back(b);
        m_indices.push_back(c);
        
        m_materials_id.push_back(material_id);
        m_smooth_groups.push_back(smooth_group);
    }
    
    //! renvoie la bbox du mesh.
    BBox& bbox( )
    {
        return m_bbox;
    }

    const BBox& getBBox( ) const
    {
        return m_bbox;
    }
    
    //! renvoie le nombre de triangles du maillage.
    int triangleCount( ) const
    {
        return (int) m_indices.size() / 3;
    }
    
    //! renvoie le nombre d'indices du maillage.
    int indiceCount( ) const
    {
        return (int) m_indices.size();
    }
    
    //! renvoie les indices du maillage.
    //~ const std::vector<int>& indices( )
    std::vector<int>& indices( )
    {
        return m_indices;
    }
    
    //! renvoie les indices du maillage.
    const std::vector<int>& indices( ) const
    {
        return m_indices;
    }
    
    //! ajoute un submesh.
    void pushSubMesh( const int begin, const int end, const int material_id )
    {
        m_submeshes.push_back( SubMesh(begin, end, material_id) );
    }

    //! renvoie le nombre de submesh.
    int subMeshCount( ) const
    {
        return (int) m_submeshes.size();
    }
    
    //! renvoie la matiere d'un submesh.
    const MeshMaterial& subMeshMaterial( const int id ) const
    {
        const int material_id= m_submeshes[id].material_id;
        if(material_id < 0)
            return m_default_material;
        else
            return *m_materials[material_id];
    }

    //! renvoie la matiere d'un submesh.
    const MeshMaterial& subMeshMaterial( const int id )
    {
        const int material_id= m_submeshes[id].material_id;
        if(material_id < 0)
            return m_default_material;
        else
            return *m_materials[material_id];
    }
    
    //! renvoie les parties du maillage.
    const std::vector<SubMesh>& subMeshes( )
    {
        return m_submeshes;
    }

    //! renvoie les parties du maillage.
    const std::vector<SubMesh>& subMeshes( ) const
    {
        return m_submeshes;
    }
    
    int pushDefaultMaterial( )
    {
        m_materials.push_back(&m_default_material);
        return (int) m_materials.size() - 1;
    }
    
    //! remplace l'ensemble de matieres.
    void setMaterials( const std::vector<MeshMaterial *>& materials )
    {
        m_materials= materials;
    }
    
    //! renvoie le nombre de matieres.
    int materialCount( ) const
    {
        return (int) m_materials.size();
    }
    
    //! renvoie l'ensemble de matieres.
    const std::vector<MeshMaterial *>& materials( )
    {
        return m_materials;
    }
    
    //! renvoie l'ensemble de matieres.
    const std::vector<MeshMaterial *>& materials( ) const
    {
        return m_materials;
    }
    
    //! renvoie l'ensemble d'identifiants de matieres des triangles.
    const std::vector<int>& triangleMaterialsId( ) const
    {
        return m_materials_id;
    }
    
    //! renvoie la matiere d'un triangle.
    MeshMaterial& triangleMaterial( const int id )
    {
        const int material_id= m_materials_id[id];
        
        if(material_id < 0)
            return m_default_material;
        else
            return material(material_id);
    }
    
    //! renvoie la matiere d'un triangle.
    const MeshMaterial& triangleMaterial( const int id ) const
    {
        const int material_id= m_materials_id[id];
        
        if(material_id < 0)
            return m_default_material;
        else
            return material(material_id);
    }

    //! renvoie l'indice de la matiere d'un triangle.
    int getTriangleMaterialId( const int id ) const
    {
        return m_materials_id[id];
    }
    
    //! renvoie un triangle 'geometrique' pour le calcul d'intersection avec un rayon.
    //! utiliser les resultats de l'intersection pour calculer la normale et les texcoords interpolees au point d'intersection.
    //! cf. Mesh::getUVPoint(), Mesh::getUVNormal(), Mesh::getUVTexCoord().
    Triangle getTriangle( const int id ) const
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        
        return Triangle(a, b, c);
    }
    
    //! renvoie un pn triangle.
    PNTriangle getPNTriangle( const int id ) const
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        
        if(m_normals.size() != m_positions.size())
        {
            // pas de normales associees aux sommets, calcule la normale geometrique.
            const Vector ab(a, b);
            const Vector ac(a, c);
            Normal nn( Normalize(Cross(ab, ac)) );
            
            return PNTriangle(a, nn, b, nn, c, nn);
        }
        
        // renvoie les normales associees aux sommets du triangle
        const Normal& na= normal(m_indices[3*id]);
        const Normal& nb= normal(m_indices[3*id +1]);
        const Normal& nc= normal(m_indices[3*id +2]);
        return PNTriangle(a, na, b, nb, c, nc);
    }
    
    //! renvoie la boite englobante d'un triangle.
    BBox getTriangleBBox( const int id )
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        
        BBox bbox;
        bbox.Union(a);
        bbox.Union(b);
        bbox.Union(c);
        
        return bbox;
    }

    //! renvoie l'aire d'un triangle.
    float getTriangleArea( const int id )
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        const Vector ab(a, b);
        const Vector ac(a, c);
        return .5f * Cross(ab, ac).Length();
    }
    
    //! calcule et renvoie la normale geometrique d'un triangle.
    Normal getTriangleNormal( const int id ) const
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        const Vector ab(a, b);
        const Vector ac(a, c);
        
        if(ab.LengthSquared() == 0.f || ac.LengthSquared() == 0.f)
            return Normal();
        
        return Normal(Normalize(Cross(ab, ac)));
    }
    
    //! interpole une position a l'interieur d'un triangle, connaissant les coordonnees barycentriques du point.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Point getUVPoint( const int id, const float u, const float v ) const
    {
        const Point& a= position(m_indices[3*id]);
        const Point& b= position(m_indices[3*id +1]);
        const Point& c= position(m_indices[3*id +2]);
        
        const float w= 1.f - u - v;
        return a * w + b * u + c * v;
    }
    
    //! interpole une normale a l'interieur d'un triangle, connaissant ses coordonnees barycentriques.
    //! convention n(u, v)= (1 - u - v) * a + u * b + v * c
    Normal getUVNormal( const int id, const float u, const float v ) const
    {
        if(m_normals.empty())
            // renvoie la normale geometrique, si les normales des sommets n'existent pas
            return getTriangleNormal(id);
        
        const Normal& a= normal(m_indices[3*id]);
        const Normal& b= normal(m_indices[3*id +1]);
        const Normal& c= normal(m_indices[3*id +2]);
        
        const float w= 1.f - u - v;
        return a * w + b * u + c * v;
    }
    
    //! interpole une coordonnee de texture a l'interieur du triangle, connaissant ses coordonnees barycentriques.
    //! convention t(u, v)= (1 - u - v) * a + u * b + v * c
    Point2 getUVTexCoord( const int id, const float u, const float v ) const
    {
        if(m_texcoords.empty())
            // pas de coordonnee de textures dans le maillage.
            return Point2();
        
        const Point2& a= texcoords(m_indices[3*id]);
        const Point2& b= texcoords(m_indices[3*id +1]);
        const Point2& c= texcoords(m_indices[3*id +2]);
        
        const float w= 1.f - u - v;
        return a * w + b * u + c * v;
    }
    
    //! renvoie le smooth group du triangle.
    int getTriangleSmoothGroup( const int id )
    {
        if(m_smooth_groups.empty())
            return -1;
        return m_smooth_groups[id];
    }
    
    //! construit la liste d'adjacence des sommets (liste de triangles).
    int buildAdjacency( );
    
    //! construit les smooth groups en fonction de l'angle entre les normales des triangles adjacents.
    int buildNormalSmoothGroups( const float max_angle );
    
    //! construit les smooth groups en fonction de la distance entre les coordonnees de textures des triangles adjacents. 
    int buildTexCoordSmoothGropus( const float max );
    
    //! construit les normales du maillage. 
    //! \todo utiliser les smooth groups s'ils sont presents.
    int buildNormals( );
};

} // namespace

#endif
