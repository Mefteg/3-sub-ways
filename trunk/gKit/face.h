#ifndef __FACE__
#define __FACE__

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "utils.h"

#include "Geometry.h"

class Vertex;
class Halfedge;

using namespace std;

class Face {
    public:
        Halfedge * he;
        Face();
        Face(Halfedge *);

        //retourne les halfedges qui composent la face
        vector<Halfedge *> getHalfedges();
        //retourne les vertex qui composent la face
        vector<Vertex *> getVertex();

        gk::Vector computeNormal();

        //ecrit la face au format obj
        string toObj();

        void print();

        static void fromObj(string, vector<Vertex *> *,
                            vector<Halfedge *> *,
                            vector<Face *> *);
};

#endif

