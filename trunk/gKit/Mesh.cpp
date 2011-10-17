
#include "Mesh.h"

namespace gk {

//! construit la liste d'adjacence des sommets (liste de triangles).
int Mesh::buildAdjacency( )
{
    /*         
        la tete de la liste d'adjacence du sommet d'indice a est m_position_adjacency[a]
        toutes les listes sont concatenees dans une liste 'globale' m_adjacency
        
        la premiere face incidente au sommet a est m_adjacency[ m_position_adjacency[a] ]
        les autres faces incidentes se trouvent dans les elements suivants de m_adjacency,
        la fin de la liste est marquee par un numero de face = -1
        
        exemple de parcours pour le sommet d'indice a:
        for(int i= m_position_adjacency[a];
            m_adjacency[i] != -1;
            i++;
        {
            le triangle d'indice m_adjacency[i] est adjacent / incident au sommet a
        }
     */
    
    const int triangles_n= triangleCount();
    const int positions_n= positionCount();
    
    // initialise la liste d'adjacence des sommets
    m_position_adjacency.clear();
    m_position_adjacency.resize(positions_n, 0);
    
    // passe 1 : compte le nombre de faces partageant chaque sommet
    {
        for(int i= 0; i < triangles_n; i++)
        {
            const int a= m_indices[3*i];
            m_position_adjacency[a]++;
            
            const int b= m_indices[3*i +1];
            m_position_adjacency[b]++;
            
            const int c= m_indices[3*i +2];
            m_position_adjacency[c]++;
        }
    }
    
    // passe 2 : distribue le premier element de la liste d'adjacence dans la liste globale
    {
        int head= 0;
        int next= 0;
        for(int i= 0; i < positions_n; i++)
        {
            m_position_adjacency[i]++;      // reserve une place pour le marqeur de fin de liste
            next= head + m_position_adjacency[i];
            m_position_adjacency[i]= head;
            
            head= next;
        }
        
        // alloue la liste globale d'adjacence
        m_adjacency.clear();
        m_adjacency.resize(next);
    }
    
    // passe 3 : construit la liste d'adjacence
    {
        std::vector<int> last;
        last.resize(positions_n);
        {
            // construit la queue de liste
            for(int i= 0; i < positions_n; i++)
            {
                last[i]= m_position_adjacency[i];
                m_adjacency[last[i]]= -1;
            }
        }
        
        // insere chaque triangle dans la liste d'adjacence de ses sommets
        for(int i= 0; i < triangles_n; i++)
        {
            const int a= m_indices[3*i];
            m_adjacency[last[a]++]= i;
            m_adjacency[last[a]]= -1;       // termine la liste 
            
            const int b= m_indices[3*i +1];
            m_adjacency[last[b]++]= i;
            m_adjacency[last[b]]= -1;       // termine la liste 
            
            const int c= m_indices[3*i +2];
            m_adjacency[last[c]++]= i;
            m_adjacency[last[c]]= -1;       // termine la liste 
        }
    }
    
    return 0;
}

//! construit les smooth groups en fonction de l'angle entre les normales des triangles adjacents.
int Mesh::buildNormalSmoothGroups( const float max_angle )
{
    return -1;
}

//! construit les smooth groups en fonction de la distance entre les coordonnees de textures des triangles adjacents. 
int Mesh::buildTexCoordSmoothGropus( const float max )
{
    return -1;    
}

//! construit les normales du maillage. 
int Mesh::buildNormals( )
{
#ifdef VERBOSE
    printf("building normals...\n");
#endif
    
    // initialise les normales des sommets
    m_normals.clear();
    m_normals.resize(m_positions.size());
    
    // etape 1 : accumule les normales des triangles sur chaque sommet de chaque triangle
    {
        const int n= triangleCount();
        for(int i=0; i < n; i++)
        {
            // accumule la normale geometrique du triangle sur les normales des 3 sommets
            const int ai= m_indices[3*i];
            const int bi= m_indices[3*i +1];
            const int ci= m_indices[3*i +2];
            
            const Point& a= position(ai);
            const Point& b= position(bi);
            const Point& c= position(ci);
            const Vector ab(a, b);
            const Vector ac(a, c);
            Normal normal= Normal( Cross(ab, ac) );

            const float length= normal.Length();
            const float area= .5f * length;
            // normale ponderee par l'aire du triangle et normalisee
            const float w= area / length;
            // a verifier ... w= 0.5 !!
            
            m_normals[ai]+= normal * w;
            m_normals[bi]+= normal * w;
            m_normals[ci]+= normal * w;
        }
    }
    
    // etape 2 : normalise les normales des sommets
    {
        const int n= (int) m_normals.size();
        for(int i= 0; i < n; i++)
            m_normals[i]= Normalize(m_normals[i]);
    }
    
    return 0;
}

}
