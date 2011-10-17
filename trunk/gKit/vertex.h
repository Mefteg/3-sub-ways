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
        int ind;
        Vertex();
        Vertex(gk::Point, Halfedge * h=NULL, int i=0);

        //return true if the vertex belongs to the face, false else
        bool belongsToFace(Face *);

        //retourne toutes les faces possedant le vertex
        vector<Face *> getFaces(vector<Face *> *);

        string toObj();

        static void fromObj( string, vector<Vertex *> * );
};

#endif
