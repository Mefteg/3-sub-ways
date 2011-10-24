#ifndef __VERTEX__
#define __VERTEX__

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

#include "face.h"

class Halfedge;

using namespace std;

class Vertex {
    public:
        gk::Point v; //position
        Halfedge * he;
        gk::Vector n; //normal
        int ind; //indique la place du vertex dans un fichier obj ( lors de l'export )
        int id; //indique le numéro du vertex
        bool done;

        Vertex();
        Vertex(gk::Point, Halfedge * h=NULL, int i=0);
        Vertex(Vertex * v);

        static int ID;

        //return true if the vertex belongs to the face, false else
        bool belongsToFace(Face *);

        //retourne toutes les faces possedant le vertex
        vector<Face *> getFaces(vector<Face *> *);

        // teste si un vertex est dans un vecteur
        bool isIn(vector<Vertex*> vector);
        //retourne vrai si le vertex est sur une bordure
        bool isOnBorder();

        // retourne tout les vertex voisins
        vector<Vertex *> getNeighbours();

        string toObj();

        static void fromObj( string, vector<Vertex *> * );
};

#endif
