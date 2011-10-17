
#ifndef _GK_EFFECT_H
#define _GK_EFFECT_H

#include <string>
#include <vector>

#include "IOResource.h"
#include "TextFile.h"


namespace gk {

//! charge un fichier texte decoupe en sections (nommees). utilise pour grouper tous les shaders d'un programme, par exemple. 
//! un fichier effet (extension .gkfx) est un ensemble de paires (cle, valeur). cle est le nom des sections, valeur est le contenu des sections. 
//! une section est representee par un TextFile qui est egalement un ensemble de paires (cle, valeur).
//! cf effect_main.cpp pour un exemple complet.
class Effect : public IOResource
{
    std::vector<TextFile *> m_text;
    
    std::string m_last_line;
    std::string m_last_section;
    int m_line_number;
    char m_last_char;
    
    int read_line( FILE *in );
    int read_section( FILE *in );
    
public:
    //! constructeur par defaut.
    Effect( );
    //! constructeur, charge le fichier 'filename'.
    Effect( const std::string& filename );

    //! destructeur.
    ~Effect( );
    
    //! charge un fichier gkfx.
    int read( const std::string& filename );

    //! recherche une section d'apres son nom / sa cle. renvoie le contenu de la section, un TextFile.
    //! renvoie TextFile::notFound() en cas d'erreur / si la cle n'existe pas.
    TextFile *find( const std::string& key ) const;
};


}

#endif
