#include "vertex.h"
#include "halfedge.h"

int Vertex::ID=0;

Vertex::Vertex() {
    this->id = Vertex::ID;
    Vertex::ID++;
}

Vertex::Vertex(gk::Point v, Halfedge * h, int i) {
    this->v=v;
    this->he=h;
    this->ind=i;
    this->n=gk::Vector(0,0,0);
    this->id = Vertex::ID;
    Vertex::ID++;
}

bool Vertex::belongsToFace( Face * f ) {
    vector<Vertex *> v_V = f->getVertex();
    bool found = false;
    int i=0;
    while ( i<v_V.size() && found ) {
        if ( this == v_V.at(i) ) {
            found = true;
        }
        i++;
    }

    return found;
}

vector<Face *> Vertex::getFaces( vector<Face *> * v_Face ) {
    vector<Face *> v_F = vector<Face *>();

    for ( int i=0; i<v_Face->size(); i++ ) {
        Face * f = v_Face->at(i);
        if ( this->belongsToFace(f) ) {
            v_F.push_back(f);
        }
    }

    return v_F;
}

string Vertex::toObj() {
    ostringstream oss;
    oss << "v " << this->v[0] << " " << this->v[1] << " " << this->v[2] << endl;
    oss << "vn " << this->n[0] << " " << this->n[1] << " " << this->n[2] << endl;

    return oss.str();
}

void Vertex::fromObj( string filename, vector<Vertex *> * v_Vertex ) {
    ifstream file(filename.c_str());

    string line;
    int cpt_v=1;
    //si le fichier est bien ouvert
    if ( !file.fail() ) {
        //tant qu'il reste des choses à traiter
        while ( file ) {
            getline(file, line);
            //si le début de la ligne commencer par v
            if ( line[0] == 'v' && line[1] == ' ' ) {
                vector<string> res = vector<string>();
                Utils::explode( line, " ", &res );
                float t[3];
                for ( int i=1; i<res.size(); i++ ) { //je ne touche pas à "n"
                    t[i-1] = Utils::stringToFloat(res.at(i));
                }
                v_Vertex->push_back( new Vertex(gk::Point(t[0],t[1],t[2]), NULL, cpt_v) );
                cpt_v++;
            }
        }
        file.close();
    }
}
