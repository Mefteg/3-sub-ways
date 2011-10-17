#include "utils.h"

void Utils::explode(string chaine, string separateur, vector<string> * resultat) {
    resultat->clear();
    int found;

    found = chaine.find_first_of(separateur);

    while(found != string::npos)
    {

        if(found > 0)
        {
            resultat->push_back(chaine.substr(0,found));
        }

        chaine = chaine.substr(found+1);
        found = chaine.find_first_of(separateur);
    }

    if(chaine.length() > 0)
    {
        resultat->push_back(chaine);
    }
}

int Utils::stringToInt( string s ) {
    // créer un flux à partir de la chaîne à convertir
    std::istringstream iss( s );
    // convertir en un int
    int nombre;
    iss >> nombre; // nombre vaut 10

    return nombre;
}

float Utils::stringToFloat( string s ) {
    // créer un flux à partir de la chaîne à convertir
    std::istringstream iss( s );
    // convertir en un int
    float nombre;
    iss >> nombre; // nombre vaut 10

    return nombre;
}
