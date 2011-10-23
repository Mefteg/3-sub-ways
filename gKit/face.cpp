#include "face.h"
#include "halfedge.h"

Face::Face(){
};

Face::Face(Halfedge * h ) {
    he=h;
}

vector<Halfedge *> Face::getHalfedges() {
    vector<Halfedge *> v_H = vector<Halfedge *>();
    //je stocke la premiere arete
    v_H.push_back(this->he);
    //et je passe à la secp,de
    Halfedge * h = this->he->he_n;
    //tant que je suis pas revenu sur la premiere arete
    while ( h != this->he ) {
        v_H.push_back(h);
        //je passe à l'arete suivante
        h = h->he_n;
    }

    return v_H;
}

vector<Vertex *> Face::getVertex() {
    vector<Vertex *> v_V = vector<Vertex *>();
    v_V.push_back(this->he->v);
    Halfedge * h = this->he->he_n;
    while ( h != this->he ) {
        v_V.push_back(h->v);
        h = h->he_n;
    }

    return v_V;
}

gk::Vector Face::computeNormal() {
    vector<Vertex *> v_V = this->getVertex();
    gk::Vector u = v_V.at(1)->v - v_V.at(0)->v;
    gk::Vector v = v_V.at(2)->v - v_V.at(0)->v;
    gk::Vector norm = gk::Cross(u,v)*-1;

    return norm;
}

string Face::toObj() {
    Halfedge * he1 = this->he;
    Halfedge * he2 = he1->he_n;
    Halfedge * he3 = he2->he_n;

    ostringstream oss;
    oss << "f ";
    vector<Vertex *> v_V = this->getVertex();
    //pour chaque sommet de la face
    for ( int i=0; i<v_V.size(); i++ ) {
        //j'ecris l'indice qui lui est associé
        Vertex * v = v_V.at(i);
        oss << v->ind << "//" << v->ind << " "; // ind_vertex//ind_normale
    }
    oss << endl;

    return oss.str();
}

void Face::print() {
    vector<Halfedge *> v_H = this->getHalfedges();

    for ( int i=0; i<v_H.size(); i++ ) {
        Halfedge * h = v_H.at(i);
        h->print();
        cout << " => ";
    }
    cout << endl;
}

void Face::fromObj( string filename, vector<Vertex *> * v_Vertex, vector<Halfedge *> * v_Halfedge, vector<Face *> * v_Face ) {
    ifstream file(filename.c_str());

    string line;
    int read = 1;
    //si le fichier est bien ouvert
    if ( !file.fail() ) {
        //tant qu'il reste des choses à traiter
        while ( file ) {
            getline(file, line);
            //si le début de la ligne commencer par f
            if ( line[0] == 'f' ) {
                //je crée la face
                Face * f = new Face();
                v_Face->push_back(f);

                vector<string> res = vector<string>();
                vector<Vertex *> v_Sommet = vector<Vertex *>();
                Utils::explode(line, " ", &res);
                for ( int i=1; i<res.size(); i++ ) { //on ne prend pas la string "f "
                    vector<string> ind = vector<string>();
                    Utils::explode(res.at(i), "/", &ind);
                    // je ne m'occupe que du point pour l'instant ( et pas de la matière et de la normale )
                    v_Sommet.push_back( v_Vertex->at( Utils::stringToInt( ind.at(0) )-1 ) );
                }
                //je crée les halfedges en fonction des points de la face
                Halfedge * h = new Halfedge( v_Sommet.at(0), NULL, NULL, f );
                v_Halfedge->push_back( h );
                //je donne cette halfedge à la face
                f->he = h;
                for ( int i=1; i<v_Sommet.size(); i++ ) {
                    Halfedge * hc = new Halfedge( v_Sommet.at(i), NULL, NULL, f );
                    v_Halfedge->push_back( hc );
                    h->he_n = hc;
                    h = hc;
                }
                //je ferme la face
                h->he_n = f->he;
            }
        }
        file.close();
    }
}
