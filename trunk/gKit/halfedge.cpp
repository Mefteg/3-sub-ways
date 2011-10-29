#include "halfedge.h"

int Halfedge::ID=0;

Halfedge::Halfedge() {
    this->done = false;
    this->id = Halfedge::ID;
    Halfedge::ID++;
}

Halfedge::Halfedge(Vertex * v,Halfedge * h1=NULL,Halfedge * h2=NULL,Face * f=NULL) {
    this->v=v;
    if ( v->he == NULL ) {
        v->he = this;
    }
    he_n=h1;
    he_e=h2;
    this->f=f;
    this->done = false;
    this->id = Halfedge::ID;
    Halfedge::ID++;
}

void Halfedge::getVertex(Vertex ** a, Vertex ** b) {
    *a = this->getPrevious()->v;
    *b = this->v;
}

Vertex * Halfedge::getOrigin() {
    Halfedge * h = this->he_e;
    if ( h == NULL ) {
        h = this->getPrevious();
    }
    return h->v;
}

Halfedge * Halfedge::getPrevious() {
    Halfedge * h = this->he_n, * ho;
    while ( h != this ) {
        ho = h;
        h = h->he_n;
    }

    return ho;
}

void Halfedge::maillageToHalfedge( Vertex ** t_Maillage, int nb_pas, vector<Halfedge *> * v_Halfedge, vector<Face *> * v_Face ) {
    Halfedge ** t_He2_b = (Halfedge **) malloc(sizeof(Halfedge*)*nb_pas);
    for ( int i=0; i<nb_pas-2; i++ ) {
        Halfedge * he_1b = NULL;
        for ( int j=0; j<nb_pas-2; j++ ) {
            v_Halfedge->push_back(new Halfedge(t_Maillage[i*nb_pas+j]));
            Halfedge * he_1 = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[i*nb_pas +j]->he = he_1;

            v_Halfedge->push_back(new Halfedge(t_Maillage[i*nb_pas+j+1]));
            Halfedge * he_2 = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[i*nb_pas +j+1]->he = he_2;

            v_Halfedge->push_back(new Halfedge(t_Maillage[(i+1)*nb_pas+j]));
            Halfedge * he_3 = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[(i+1)*nb_pas +j]->he = he_3;

            he_1->he_n = he_2;
            he_2->he_n = he_3;
            he_3->he_n = he_1;

            v_Face->push_back(new Face(he_1));
            Face * f1 = v_Face->at(v_Face->size()-1);
            he_1->f = f1;
            he_2->f = f1;
            he_3->f = f1;

            if ( j > 0 ) {
                he_1->he_e = he_1b;
                he_1b->he_e = he_1;
            }
            if ( i > 0 ) {
                he_2->he_e = t_He2_b[j];
                t_He2_b[j]->he_e = he_2;
            }

            v_Halfedge->push_back(new Halfedge(t_Maillage[(i+1)*nb_pas+j+1]));
            he_1b = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[(i+1)*nb_pas+j+1]->he = he_1b;

            v_Halfedge->push_back(new Halfedge(t_Maillage[(i+1)*nb_pas+j]));
            Halfedge * he_2b = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[(i+1)*nb_pas+j]->he = he_2b;

            v_Halfedge->push_back(new Halfedge(t_Maillage[i*nb_pas+j+1]));
            Halfedge * he_3b = v_Halfedge->at(v_Halfedge->size()-1);
            t_Maillage[i*nb_pas+j+1]->he = he_3b;

            he_1b->he_n = he_2b;
            he_2b->he_n = he_3b;
            he_3b->he_n = he_1b;

            v_Face->push_back(new Face(he_1b));
            Face * f2 = v_Face->at(v_Face->size()-1);
            he_1b->f = f2;
            he_2b->f = f2;
            he_3b->f = f2;

            he_3->he_e = he_3b;
            he_3b->he_e = he_3;

            t_He2_b[j] = he_2b;
        }
    }
    free(t_He2_b);
}

void Halfedge::computeNormals(Vertex ** t_Maillage, int nb_pas, vector<Face *> * v_Face) {
    for ( int i=0; i<v_Face->size(); i++ ) {
        gk::Vector norm = v_Face->at(i)->computeNormal();
        vector<Vertex *> v_V = v_Face->at(i)->getVertex();
        for ( int j=0; j<v_V.size(); j++ ) {
            v_V.at(j)->n += gk::Normalize(norm);
        }
    }
}

void evenHalfedge( Vertex * a, Vertex * b, Halfedge * he1, Halfedge * he2 ) {
    if ( a->he != he1 && b->he != he2 ) {
        if ( a->he->he_n->v == b ) {
            he2->he_e = b->he;
        }
        else if ( b->he->he_n->v == a ) {
            he1->he_e = a->he;
        }
    }
}

int Halfedge::compareHalfedge( Halfedge * h ) {
    Vertex * a=this->v, * b;
    b = this->getPrevious()->v;
    Vertex * ah=h->v, * bh;
    bh = h->getPrevious()->v;

    if ( a == ah && b == bh ) {
        return 1;
    }
    else if ( a == bh && b == ah ) {
        return -1;
    }
    else {
        return 0;
    }
}

void Halfedge::inverseFace() {
    Halfedge * h1 = this;
    Halfedge * h2 = h1->he_n;
    Halfedge * h3 = h2->he_n;

    Vertex * a = h1->v;
    Vertex * b = h2->v;
    Vertex * c = h3->v;

    h1->v = c;
    h2->v = a;
    h3->v = b;

    a->he = h2;
    b->he = h3;
    c->he = h1;
}

Halfedge * Halfedge::subdivise() {
    Vertex * o = this->getOrigin();
    gk::Point p = (o->v + this->v->v)*0.5;
    Vertex * v = new Vertex( p, this );
    Halfedge * h2 = new Halfedge( this->v, this->he_n, NULL, this->f );
    h2->v->he = h2;
    this->v = v;
    this->he_n = h2;

    return h2;
}

void Halfedge::print() {
    cout << this->id << "->v" << this->v->id;
    if ( this->he_e != NULL ) {
        cout << " (<-> " << this->he_e->id << "->v" << this->he_e->v->id << ") ";
    }
    cout << endl;
}

void Halfedge::importFromObj(string filename, vector<Vertex *> * v_Vertex, vector<Halfedge * > * v_Halfedge, vector<Face *> * v_Face ) {
    ifstream file(filename.c_str());

    //on commence par recuperer tous les points
    Vertex::fromObj( filename, v_Vertex );
    //puis toutes les faces ( et les aretes par la meme occasion
    Face::fromObj( filename, v_Vertex, v_Halfedge, v_Face );

    //on va maintenant chercher les aretes adjacentes
    for ( int i=0; i<v_Face->size(); i++ ) {
        Face * f = v_Face->at(i);
        vector<Halfedge *> v_H1 = f->getHalfedges();
        //pour chaque arete de la face
        for ( int ind_h=0; ind_h<v_H1.size(); ind_h++ ) {
            Halfedge * hi = v_H1.at(ind_h);
            //si l'arete n'a pas d'arete adjacente
            if ( hi->he_e == NULL ) {
                //pour chaque face
                for ( int j=0; j<v_Face->size(); j++ ) {
                    //on ne test pas la même face
                    if ( j != i ) {
                        vector<Halfedge *> v_H2 = v_Face->at(j)->getHalfedges();
                        //pour chaque arete de la face
                        for ( int k=0; k<v_H2.size(); k++ ) {
                            //je regarde s'il y a des aretes en commun
                            Halfedge * hj = v_H2.at(k);
                            int d = hi->compareHalfedge(hj);
                            //si les aretes sont voisines
                            if ( d != 0 ) {
                                //je leur donne leur voisine
                                hi->he_e = hj;
                                hj->he_e = hi;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Halfedge::exportToObj( string filename, Vertex ** t_Maillage, int nb_pas, vector<Face *> * v_Face ) {
    ofstream file(filename.c_str());

    if ( !file.fail() ) {
        file << "# " << filename << endl;
        file << "#" << endl;
        file << "g surface" << endl;

        int cpt=1;
        for ( int i=0; i<nb_pas-1; i++ ) {
            for ( int j=0; j<nb_pas-1; j++ ) {
                file << t_Maillage[i*nb_pas+j]->toObj();
                t_Maillage[i*nb_pas+j]->ind = cpt;
                cpt++;
            }
        }

        file << endl;

        for ( int i=0; i<v_Face->size(); i++ ) {
            Face * f = v_Face->at(i);
            file << f->toObj();
        }

        file.close();
    }
}

void Halfedge::exportToObj( string filename, vector<Vertex *> * v_Vertex, vector<Face *> * v_Face ) {
    ofstream file(filename.c_str());

    if ( !file.fail() ) {
        file << "# " << filename << endl;
        file << "#" << endl;
        file << "g surface" << endl;

        int cpt=1;
        //Surement un probleme d'indice de Vertex
        for ( int i=0; i<v_Vertex->size(); i++ ) {
            file << v_Vertex->at(i)->toObj();
            v_Vertex->at(i)->ind = cpt;
            cpt++;
        }

        file << endl;

        for ( int i=0; i<v_Face->size(); i++ ) {
            Face * f = v_Face->at(i);
            file << f->toObj();
        }

        file.close();
    }
}

