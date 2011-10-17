
#include <cstdio>

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>

#include "Geometry.h"
#include "MeshMaterial.h"
#include "MeshMaterialIO.h"
#include "Mesh.h"
#include "MeshOBJ.h"

#include "IOFileSystem.h"
#include "ProfilerClock.h"

namespace gk
{

bool isMeshOBJ( const std::string& filename )
{
    const char *pos= strrchr(filename.c_str(), '.');
    if(pos == NULL)
        return false;
    return (strcmp(pos, ".obj") == 0);
}

namespace OBJ
{

//! parser pour analyser les fichiers obj et mtl.
class Parser
{
    FILE *m_in;
    std::string m_token;
    
    bool is_token( const char c )
    {
        return ( isalnum( c ) || c == '_' || c == '-' || c == '.' );
    }
    
public:
    Parser( const std::string& filename )
            :
            m_in( NULL ),
            m_token()
    {
        m_in = fopen( filename.c_str(), "rt" );
    }
    
    ~Parser( )
    {
        if ( m_in != NULL )
            fclose( m_in );
    }
    
    bool isValid( ) const
    {
        return ( m_in != NULL );
    }
    
    //! lecture sequentielle d'un 'mot' dans le fichier.
    //! un mot est un ensemble de caracteres quelconques delimite par des separateurs.
    //! renvoie le separateur, si la lecture d'un mot est impossible.
    int readToken( )
    {
        if ( !isValid() )
            return EOF;
            
        // sauter les blancs
        char c = fgetc( m_in );
        while ( c != EOF && ( c == ' ' || c == '\t' ) )
            c = fgetc( m_in );
            
        if ( c == '\r' )
            c = '\n'; // meme gestion des fins de lignes pour linux, mac et windows
            
        // lire tous les caracteres alphanumeriques
        m_token.resize( 0 );
        while ( c != EOF && is_token( c ) )
        {
            m_token.push_back( c );
            
            c = fgetc( m_in );
            if ( c == '\r' )
                c = '\n'; // meme gestion des fins de lignes pour linux, mac et windows
        }
        
        // separateurs
        if ( c == '#' )
        {
            if ( m_token.empty() )
                m_token.push_back( c );
            else
                ungetc( c, m_in );
        }
        else if ( c == '/' ) // ambiguite sur les noms de fichiers ...
            m_token.push_back( c );
            
        // indiquer la fin de la ligne ou du fichier
        if ( m_token.empty() )
            return c;
            
        // forcer une fin de ligne avant la fin du fichier
        if ( c == EOF )
            ungetc( '\n', m_in );
            
        else if ( c == '\n' )
            ungetc( c, m_in );
            
        return 0;
    }
    
    //! lecture de la fin de la ligne, renvoie la chaine de caracteres.
    int readString( )
    {
        std::string string;
        
        // concatene tous les tokens jusqu'a la fin de la ligne
#if 0
        while ( readToken() != '\n' )
            string += m_token;
#else
        // ajoute aussi les separateurs dans la chaine ...
        int code = readToken();
        while ( code != '\n' )
        {
            string.append( m_token );
            if ( code != 0 )
                string.push_back( code );
            code = readToken();
        }
#endif
            
        assert( string.length() == strlen( string.c_str() ) );
        
        m_token.swap( string );
        if ( m_token.empty() )
            return -1;
        else
            return 0;
    }
    
    //! renvoie le dernier 'mot' lu par readToken().
    const std::string& getToken( ) const
    {
        return m_token;
    }
    
    //! renvoie le dernier caractere du dernier 'mot' lu.
    char getLastChar( ) const
    {
        const int length = ( int ) m_token.length();
        if ( length == 0 )
            return 0;
            
        return m_token[length -1];
    }
    
    //! renvoie un caractere du dernier 'mot' lu.
    char getChar( const int i = 0 ) const
    {
        assert( i >= 0 && i < ( int ) m_token.length() );
        return m_token[i];
    }
    
    //! converti le dernier 'mot' lu en reel.
    int getFloat( float& v ) const
    {
        v = 0.f;
        if ( sscanf( m_token.c_str(), "%f", &v ) != 1 )
            return -1;
        else
            return 0;
    }
    
    //! converti le dernier 'mot' lu en entier.
    int getInt( int& v ) const
    {
        v = 0;
        if ( sscanf( m_token.c_str(), "%d", &v ) != 1 )
            return -1;
        else
            return 0;
    }
    
    //! lit un vecteur 3.
    int readVector3( Vector& v )
    {
        int i;
        for ( i = 0; readToken() != '\n'; i++ )
            if ( i < 3 && getFloat( v[i] ) < 0 )
                return -1;
                
        if ( i != 3 )
            return -1;
        else
            return 0;
    }
    
    //! lit un vecteur 2.
    int readVector2( Point2& v )
    {
        int i;
        for ( i = 0; readToken() != '\n'; i++ )
            if ( i < 2 && getFloat( v[i] ) < 0 )
                return -1;
                
        if ( i != 2 )
            return -1;
        else
            return 0;
    }
    
    //! saute la ligne.
    //! lecture de tous les 'mots' jusqu'a la fin de la ligne.
    int skipLine( )
    {
        while ( readToken() != '\n' )
        {
        #ifdef VERBOSE_DEBUG
            printf("skip '%s'\n", getToken().c_str());
        #endif
        }
            
        return 0;
    }
};

//! determine l'indice d'un attribut de sommet dans le maillage.
//! renvoie 0 si l'attribut n'existe pas, 1 si l'attribut existe et -1 en cas d'erreur.
static
int getAttribute( const Parser& parser, int& attr, const int attributes_n )
{
    if ( parser.getChar( 0 ) == '/' )
        return 0;
        
    if ( parser.getInt( attr ) < 0 )
        return -1;
        
    if ( attr < 0 )
        attr += attributes_n;
    else
        attr -= 1;
        
    if ( attr < 0 || attr >= attributes_n )
        return -1;
    else
        return 1;
}


//! representation d'un sommet, indice de matiere + triplet d'indices position, normale et texcoord.
struct Vertex
{
    int m_indices[4];
    
    Vertex( ) {}
    
    Vertex( const int m, const int p, const int n, const int t )
    {
        m_indices[0] = m;
        m_indices[1] = p;
        m_indices[2] = n;
        m_indices[3] = t;
    }
    
    ~Vertex( ) {}
    
    //! comparaison de 2 sommets pour l'insertion dans une std::map.
    static
    bool less( const Vertex& a, const Vertex& b )
    {
        // definit un ordre lexicographique sur la matiere + les 3 indices : position, normale, texcoord
        for ( int i = 0; i < 4; i++ )
        {
            if ( a.m_indices[i] < b.m_indices[i] )
                // renvoie vrai lorsque a < b
                return true;
            else if ( a.m_indices[i] > b.m_indices[i] )
                return false;
        }
        
        return false;
    }
    
    //! renvoie l'indice de la matiere du sommet.
    int material( ) const
    {
        return m_indices[0];
    }
    
    //! renvoie l'indice de la position du sommet.
    int position( ) const
    {
        return m_indices[1];
    }
    
    //! renvoie l'indice de la normale du sommet.
    int normal( ) const
    {
        return m_indices[2];
    }
    
    //! renvoie l'indice de la coordonnee de texture du sommet.
    int texcoord( ) const
    {
        return m_indices[3];
    }
};

//! operateur de comparaison pour l'insertion d'un Vertex dans une std::map.
bool operator<( const Vertex& a, const Vertex& b )
{
    return Vertex::less( a, b );
}

//! representation d'un triangle <abc> pour le maillage.
struct Triangle
{
    int m_indices[3];
    int m_material_id;  // a stocker a part ... cf. construction de l'index buffer
    int m_smooth_id;
    
    //! contructeur par defaut.
    Triangle( )
            :
            m_material_id( -1 ),
            m_smooth_id( -1 )
    {}
    
    //! destructeur.
    ~Triangle( ) {}
    
    //! construit un triangle.
    Triangle( const int a, const int b, const int c )
    {
        m_indices[0] = a;
        m_indices[1] = b;
        m_indices[2] = c;
    }
    
    //! fixe l'identifiant de la matiere du triangle.
    void setMaterial( const int id )
    {
        m_material_id = id;
    }
    
    //! fixe le smooth group du triangle
    void setSmoothGroup( const int group_id )
    {
        m_smooth_id = group_id;
    }
    
    //! renvoie l'indice du sommet a du triangle.
    int a( ) const
    {
        return m_indices[0];
    }
    
    //! renvoie l'indice du sommet b du triangle.
    int b( ) const
    {
        return m_indices[1];
    }
    
    //! renvoie l'indice du sommet c du triangle.
    int c( ) const
    {
        return m_indices[2];
    }
    
    //! renvoie l'identifiant de la matiere du triangle.
    int material( ) const
    {
        return m_material_id;
    }
    
    //! renvoie l'identifiant du smooth group du triangle.
    int smoothGroup( ) const
    {
        return m_smooth_id;
    }
    
    //! comparaison de 2 sommets pour l'insertion dans une std::map
    static
    bool material_less( const Triangle& a, const Triangle& b )
    {
        return ( a.material() < b.material() );
    }
};

} // namespace OBJ


//! lit un fichier .mtl et renvoie la liste de matieres decrites dans le fichier.
int MaterialLoadFromMTL( const std::string& filename )
{
    using namespace OBJ;
    
#ifdef VERBOSE
    printf( "loading materials '%s'...\n", filename.c_str() );
#endif
    
    Parser parser( filename );
    if ( parser.isValid() == false )
        return -1;
        
    MeshMaterial *material = NULL;
    for ( ;; )
    {
        int code = parser.readToken();
        if ( code == EOF )
            break;
        if ( code == '\n' )
            continue;
            
        const std::string& token = parser.getToken();
        
        if ( token == "newmtl" )
        {
            if ( parser.readString() < 0 )
                return -1;
            
            material = MeshMaterialIO::manager().find( filename, parser.getToken() );
            if ( material == NULL )
            {
                // la matiere n'existe pas, la creer
                material = new MeshMaterial( parser.getToken() );
                MeshMaterialIO::manager().insert( material, filename, parser.getToken() );
            }
            
            continue;
        }
        
        if ( material == NULL )
            continue;
        
        if ( token == "Kd" ) // diffuse
        {
            Vector v;
            if ( parser.readVector3( v ) < 0 )
                return -1;
            material->diffuse = Energy( v.x, v.y, v.z );
        }
        
        else if ( token == "map_Kd" ) // diffuse texture
        {
            if ( parser.readString() < 0 )
                return -1;
                
            // construire le chemin d'acces de la texture
            material->diffuse_texture = IOFileSystem::pathname( filename ) + parser.getToken();
        }
        
        else if ( token == "Ks" ) // specular color
        {
            Vector v;
            if ( parser.readVector3( v ) < 0 )
                return -1;
            material->specular = Energy( v.x, v.y, v.z );
        }
        
        else if ( token == "Ns" ) // phong exp
        {
            if ( parser.readToken() < 0 )
                return -1;
            if ( parser.getFloat( material->n ) < 0 )
                return -1;
        }
        
        else if ( token == "Ni" ) // indice de refraction
        {
            if ( parser.readToken() < 0 )
                return -1;
            if ( parser.getFloat( material->ni ) < 0 )
                return -1;
        }
        
        else if ( token == "Le" ) // emission d'une source
        {
            Vector v;
            if ( parser.readVector3( v ) < 0 )
                return -1;
            material->emission= Energy( v.x, v.y, v.z );
        }
        
        else
            // commande non reconnue
            parser.skipLine();
    }
    
    return 0;
}


//! charge un maillage triangule et ses matieres a partir d'un maya .obj et .mtl.
int MeshLoadFromOBJ( const std::string& filename, Mesh *mesh )
{
    // . etape 1 : charger la geometrie
    OBJ::Parser parser( filename );
    if ( parser.isValid() == false )
        return -1;
        
#ifdef VERBOSE
    printf( "loading mesh '%s'...\n", filename.c_str() );
#endif
    
    std::vector<Point> positions;
    std::vector<Normal> normals;
    std::vector<Point2> texcoords;
    typedef std::map<OBJ::Vertex, int> vertices_map_type;
    vertices_map_type vertices_map;
    std::vector<OBJ::Vertex> vertices;
    std::vector<OBJ::Triangle> triangles;
    std::vector<int> indices;
    
    int vertices_n = 0;
    bool has_positions = false;
    bool has_normals = false;
    bool has_texcoords = false;
    
    typedef std::map<std::string, int> materials_map_type;
    materials_map_type materials_map;
    std::vector<MeshMaterial *> materials;
    int material_id = -1;
    std::string materials_lib;
    
    // enregistre la matiere par defaut, si necessaire
    MeshMaterial *default_material = MeshMaterialIO::manager().find( "", "gk_default_material" );
    if ( default_material == NULL )
    {
        default_material = new MeshMaterial( "gk_default_material" );
        MeshMaterialIO::manager().insert( default_material, "", default_material->name );
    }
    
    typedef std::map<std::string, int> smooth_map_type;
    smooth_map_type smooth_map;
    int smooth_group = -1;
    
    for ( ;; )
    {
        int code = parser.readToken();
        if ( code == EOF )
            break;
        if ( code == '\n' )
            continue;
            
        const std::string& token = parser.getToken();
        if ( token == "mtllib" )
        {
            if ( parser.readString() < 0 )
                return -1;
                
            materials_lib = IOFileSystem::pathname( filename ) + parser.getToken();
            MeshMaterialIO::read( materials_lib );
        }
        
        else if ( token == "usemtl" )
        {
            if ( parser.readString() < 0 )
                return -1;
                
            material_id = -1;
            // rechercher la matiere dans la liste de matieres deja referencees par le mesh
            materials_map_type::iterator found = materials_map.find( parser.getToken() );
            if ( found == materials_map.end() )
            {
                // rechercher la matiere dans le manager, si elle n'existe pas utiliser une matiere par defaut
                MeshMaterial *material = MeshMaterialIO::manager().find( materials_lib, parser.getToken() );
                if ( material == NULL )
                {
                    // matiere par defaut
                    found = materials_map.find( "gk_default_material" );
                    if ( found == materials_map.end() )
                    {
                        material = MeshMaterialIO::manager().find( "", "gk_default_material" );
                        assert( material != NULL );
                        
                        material_id = ( int ) materials.size();
                        materials_map.insert( std::make_pair( material->name, material_id ) );
                        materials.push_back( material );
                    }
                    else
                        material_id = found->second;
                        
                #ifdef VERBOSE_DEBUG
                    printf( "  using material '%s' (found '%s')\n", parser.getToken().c_str(), materials[material_id]->m_name.c_str() );
                #endif
                }
                else
                {
                    // referencer la matiere
                    material_id = ( int ) materials.size();
                    materials_map.insert( std::make_pair( material->name, material_id ) );
                    materials.push_back( material );
                }
            }
            else
                material_id = found->second;
                
            assert( material_id != -1 );
        #ifdef VERBOSE_DEBUG
            printf( "  using material '%s' (found '%s')\n", parser.getToken().c_str(), materials[material_id]->m_name.c_str() );
        #endif
        }
        
        else if ( token == "v" )
        {
            Vector v;
            if ( parser.readVector3( v ) < 0 )
            {
                printf("OBJ::read_vertex: error reading vertex position.\n");
                return -1;
            }
            positions.push_back( Point( v ) );
        }
        
        else if ( token == "vn" )
        {
            Vector n;
            if ( parser.readVector3( n ) < 0 )
            {
                n= Vector();
                //~ return -1;
            }
            normals.push_back( Normal( n ) );
        }
        
        else if ( token == "vt" )
        {
            Point2 t;
            if ( parser.readVector2( t ) < 0 )
            {
                t= Point2();
                //~ return -1;
            }
            texcoords.push_back( t );
        }
        
        else if ( token == "f" )
        {
            int i = 0;
            indices.resize( 0 );
            
            while ( parser.readToken() != '\n' )
            {
                // lire les attributs position / texcoord / normal
                int position_id = -1;
                int normal_id = -1;
                int texcoord_id = -1;
                
                if ( getAttribute( parser, position_id, ( int ) positions.size() ) != 1 )
                    return -1;
                has_positions = true;
                
                if ( parser.getLastChar() == '/' )
                {
                    if ( parser.readToken() == '\n' )
                        return -1;
                        
                    const int texcoord_code = getAttribute( parser, texcoord_id, ( int ) texcoords.size() );
                    if ( texcoord_code < 0 )
                        return -1;
                    if ( texcoord_code > 0 )
                        has_texcoords = true;
                        
                    if ( parser.getLastChar() == '/' )
                    {
                        if ( parser.readToken() == '\n' )
                            return -1;
                            
                        const int normal_code = getAttribute( parser, normal_id, ( int ) normals.size() );
                        if ( normal_code < 0 )
                            return -1;
                        if ( normal_code > 0 )
                            has_normals = true;
                    }
                }
                
                // conserve les indices des attributs du nouveau sommet, pour construire le vertex buffer
                /*
                    // meilleure 'gestion' des objets non homogenes :
                
                    if(has_normal && normal_id < 0)
                        default_normal_id= recuperer "gk_default_normal"
                        if(default_normal_id < 0)
                            default_normal_id= inserer "gk_default_normal"
                    normal_id= default_normal_id;
                
                    // idem pour has_texture / texture_id
                
                 */
                OBJ::Vertex vertex( material_id, position_id, normal_id, texcoord_id );
                
                // insere le sommet dans la map
                std::pair<vertices_map_type::iterator, bool> found =
                    vertices_map.insert( std::make_pair( vertex, vertices_n ) );
                if ( found.second )
                {
                    // le sommet n'existait pas, le conserver
                    vertices.push_back( vertex );
                    vertices_n += 1;
                }
                
                // conserver l'indice associe
                indices.push_back( found.first->second );
                
                i += 1;
                if ( i >= 3 )
                {
                    // trianguler automatiquement le polygone (convexe)
                    OBJ::Triangle triangle( indices[0], indices[i-2], indices[i-1] );
                    
                    if ( material_id < 0 )
                    {
                        // utiliser la matiere par defaut
                        materials_map_type::iterator found = materials_map.find( "gk_default_material" );
                        if ( found == materials_map.end() )
                        {
                            MeshMaterial *material = MeshMaterialIO::manager().find( "", "gk_default_material" );
                            assert( material != NULL );
                            
                            material_id = ( int ) materials.size();
                            materials_map.insert( std::make_pair( material->name, material_id ) );
                            materials.push_back( material );
                        }
                        else
                            material_id = found->second;
                            
                    #ifdef VERBOSE
                        printf( "  using default material\n" );
                    #endif
                    }
                    
                    assert( material_id != -1 );
                    triangle.setMaterial( material_id );
                    triangle.setSmoothGroup( smooth_group );
                    triangles.push_back( triangle );
                    
                    if((triangles.size() % 100000) == 0)
                        printf("  reading triangles %d...\n", (int) triangles.size());
                }
            }
        }
        else if ( token == "s" )
        {
            if ( parser.readToken() != '\n' )
            {
                if ( parser.getToken() == "off" || parser.getToken() == "0" )
                {
                    smooth_group = -1;
                #ifdef VERBOSE_DEBUG
                    printf( "smooth group off\n" );
                #endif
                }
                else
                {
                    std::pair<smooth_map_type::iterator, bool> found =
                        smooth_map.insert( std::make_pair( parser.getToken(), ( int ) smooth_map.size() ) );
                    // recupere l'identifiant du groupe
                    smooth_group = found.first->second;
                #ifdef VERBOSE_DEBUG
                    printf( "smooth group '%s', id %d\n", parser.getToken().c_str(), smooth_group );
                #endif
                }
                
                parser.skipLine();
            }
        }
        
        else
            // commande non reconnue
            parser.skipLine();
    }
    
#ifdef VERBOSE
    ProfilerClock::Ticks mesh_start = ProfilerClock::getTicks();
#endif
    
    // . etape 2 : construire le Mesh
    // + reordonner positions + normales + texcoords
    for ( int i = 0; i < ( int ) vertices.size(); i++ )
    {
        assert( vertices[i].position() >= 0 );
        mesh->pushPosition( positions[vertices[i].position()] );
        
        if ( has_normals )
        {
            if ( vertices[i].normal() < 0 )
                mesh->pushNormal( Normal() );
            else
                mesh->pushNormal( normals[vertices[i].normal()] );
        }
        
        if ( has_texcoords )
        {
            if ( vertices[i].texcoord() < 0 )
                mesh->pushTexCoord( Point2() );
            else
                mesh->pushTexCoord( texcoords[vertices[i].texcoord()] );
        }
    }
    
    {
        // . etape 3 : trier les triangles par matiere, construire les submeshes
        std::sort( triangles.begin(), triangles.end(), OBJ::Triangle::material_less );
        
        // inserer les triangles
        const int n = ( int ) triangles.size();
        for ( int i = 0; i < n; i++ )
        {
            const OBJ::Triangle& triangle = triangles[i];
            mesh->pushTriangle(
                triangle.a(), triangle.b(), triangle.c(),
                triangle.material(), triangle.smoothGroup() );
        }
        
        // identifier les sequences de matieres identiques
        int submesh = 0;
        int material_id = triangles[0].material();
        for ( int i = 1; i < n; i++ )
        {
            if ( triangles[i].material() != material_id )
            {
                mesh->pushSubMesh( submesh, i, material_id );
                material_id = triangles[i].material();
                submesh = i;
            }
        }
        
        mesh->pushSubMesh( submesh, n, material_id );
    }
    
    mesh->setMaterials( materials );
    
#ifdef VERBOSE
    int mesh_time = ProfilerClock::getDelay( mesh_start );
    
    printf( "  positions %d, normals %d, texcoords %d\n", mesh->positionCount(), mesh->normalCount(), mesh->texCoordCount() );
    printf( "  triangles %d\n", mesh->triangleCount() );
    printf( "  materials %d (%d)\n", mesh->subMeshCount(), ( int ) materials.size() );
    printf( "  build time %dms\n", mesh_time / 1000 );
    printf( "done.\n" );
#endif
    
    return 0;
}

} // namespace
