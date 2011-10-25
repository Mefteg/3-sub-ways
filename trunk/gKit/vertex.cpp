#include "vertex.h"
#include "halfedge.h"

int Vertex::ID=0;

Vertex::Vertex() {
    this->id = Vertex::ID;
    Vertex::ID++;
    this->done = false;
}

Vertex::Vertex(gk::Point v, Halfedge * h, int i) {
    this->v=v;
    this->he=h;
    this->ind=i;
    this->n=gk::Vector(0,0,0);
    this->id = Vertex::ID;
    Vertex::ID++;
    this->done = false;
}

Vertex::Vertex(Vertex * v) {
    this->v=v->v;
    this->he=v->he;
    this->ind=v->ind;
    this->n=v->n;
    this->id = v->id;
    this->done = v->done;
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

bool Vertex::isIn(std::vector<Vertex*> vector)
{
	for(int i = 0; i < vector.size(); ++i) if(vector.at(i) = this) return true;
	return false;
}

bool Vertex::isOnBorder() {
    Halfedge * h = this->he;
    Halfedge * hb = h;
    int cpt=0;
    while ( hb->he_e != NULL && (hb != h || cpt == 0 ) ) {
        hb = hb->he_e->getPrevious();
        cpt++;
    }

    //si on a fait le tour des aretes poitant vers le vertex
    if ( hb == h && cpt > 0 ) {
        return false;
    }
    else {
        return true;
    }
}

/*
std::vector<Vertex*> Vertex::getNeighbours()
{
	vector<Vertex*> neighbours = vector<Vertex*>();
	Halfedge* temp = this->he->he_n;

	// Sens horaire
	for(;;)
	{
		if(temp->v->isIn(neighbours))
		{
			cout << "\t\tOn part via sens horaire !\n";
			return neighbours;
		}

		neighbours.push_back(temp->v);
		
		cout << "Sens horaire\n";

		if(temp->he_e) temp = temp->he_e->he_n;
		else break;
	}
	cout << "On sort du sens horaire !\n";

	if(this->he->he_e) temp = this->he->he_e;
	else
	{
		cout << "\t\tOn part via sens trigonométrique !\n";
		return neighbours;
	}

	// Sens trigonométrique
	for(;;)
	{
		neighbours.push_back(temp->v);
		
		cout << "Sens trigonométrique\n";
		
		if(temp->getPrevious()->he_e) temp = temp->getPrevious()->he_e;
		else break;
	}
	cout << "On sort du sens trigonométrique !\n";

	return neighbours;
}
*/

std::vector<Vertex*> Vertex::getNeighbours()
{
	vector<Vertex*> neighbours = vector<Vertex*>();
	
	//je récupère les aretes qui sont sur les bordures
	Halfedge * h = this->he;
	Halfedge * hb = h;
	
	//neighbours.push_back(hb->getOrigin());
	int cpt=0;
	
	//je parcours le côté gauche
	while ( hb->he_e != NULL && (hb != h || cpt == 0 ) )
	{
		hb = hb->he_e->getPrevious();
		neighbours.push_back(hb->getOrigin());
		cpt++;
	}

	//si j'ai fait le tour de tous les voisins
	if ( hb == h ) return neighbours;

	//sinon
	else
	{
		hb = h->he_n;
		neighbours.push_back(hb->v);
		
		//je parcours le côté droit
		while ( hb->he_e != NULL && hb != h->he_n )
		{
			hb = hb->he_e->he_n;
			neighbours.push_back(hb->v);
		}

		return neighbours;
	}

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
