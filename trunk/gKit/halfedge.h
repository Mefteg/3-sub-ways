#ifndef __HALFEDGE__
#define __HALFEDGE__

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "utils.h"

#include "Geometry.h"
#include "Transform.h"

#include "Triangle.h"
#include "PNTriangle.h"

#include "vertex.h"
#include "face.h"


using namespace std;

class Halfedge {
    public:
        Vertex * v;
        Halfedge * he_n; //next
        Halfedge * he_e; //even
        Face * f;
        bool done;
        int id;

        static int ID;

        Halfedge();
        Halfedge(Vertex *,Halfedge *,Halfedge *,Face *);

        //retourne les deux vertex qui composent l'arete
        //dans ce sens : a -> b
        void getVertex(Vertex ** a, Vertex ** b);
        Vertex * getOrigin();
        //retourne l'arete precedente
        Halfedge * getPrevious();

        //retourne 0 si les aretes n'ont pas leurs sommets en commun
        //1 si les aretes ont les memes sommets et sont dans le meme sens
        //-1 si les aretes ont les memes sommets et ne sont pas dans le meme sens
        int compareHalfedge(Halfedge *);
        void inverseFace();
        //retourne la nouvelle demi-arete créée devant celle-ci ( genere donc une arete et un sommet )
        // ! la nouvelle arete ne possede pas d'arete paire !
        Halfedge * subdivise();

        static void maillageToHalfedge(Vertex **, int, vector<Halfedge *> *, vector<Face *> *);
        static void computeNormals(Vertex **, int, vector<Face *> *);

        static void importFromObj(string, vector<Vertex *> *, vector<Halfedge *> *, vector<Face *> *);
        static void exportToObj(string, Vertex **, int, vector<Face *> *);
        static void exportToObj(string, vector<Vertex *> *, vector<Face *> *);
        static string pointToObj(Vertex *);
        static string faceToObj(Face *);
};
#endif

