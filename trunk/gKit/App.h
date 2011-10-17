
#ifndef _GK_APP_H
#define _GK_APP_H

#include "SDLPlatform.h"

namespace gk {

//! application de base SDL + openGL. consultez le <a href="http://www710.univ-lyon1.fr/~jciehl/Public/SDL_PG/index.html">libSDL Programming Guide</a> 
class App
{
protected:
    unsigned char *m_key_state;
    unsigned int *m_key_map;

    int m_width, m_height;
    int m_stop;

    App( const App& );
    App& operator=( const App& );

public:
    //! \name initialisation manuelle de l'application.
    // @{
    //! construction d'une application par defaut (non initialisee).
    App( );

    //! cree une fenetre de dimension w x h et termine l'initialisation.
    int createWindow( const int w, const int h );

    //! \name initialisation complete de l'application.
    // @{
    //! construction d'une application.
    //! \param w largeur de la fenetre
    //! \param h largeur de la fenetre
    App( const int w, const int h );
    // @}

    //! destruction de l'application.
    virtual ~App( );

    //! redimensionne la fenetre.
    int resizeWindow( const int w, const int h, const unsigned int flags= 0 );

    //! renvoie les dimensions de la fenetre
    int windowWidth( ) const
    {
        return m_width;
    }

    //! renvoie les dimensions de la fenetre
    int windowHeight( ) const
    {
        return m_height;
    }

    
    //! indique si la fenetre est fermee.
    bool isClosed( );

    //! demander a fermer la fenetre.
    void Close( );
    
    //! traitement des evenements systemes : clavier, souris, fenetre, etc.
    bool processEvents( );

    //! \name interception des evenements clavier / souris / redimensionnement.
    //! App fournit une implementation par defaut qui ne fait rien 
    //! (les evenements sont reellement traites par processEvents()).
    //! a deriver, pour integrer nvWidgets, par exemple.
    
    // @{
    //! traitement des redimensionnements de la fenetre de l'application.
    virtual void processWindowResize( SDL_ResizeEvent& event )
    {
        return;
    }
    
    //! traitement des clicks souris.
    virtual void processMouseButtonEvent( SDL_MouseButtonEvent& event )
    {
        return;
    }
    
    //! traitement des deplacements de la souris.
    virtual void processMouseMotionEvent( SDL_MouseMotionEvent& event )
    {
        return;
    }
    
    //! traitement des evenements claviers.
    virtual void processKeyboardEvent( SDL_KeyboardEvent& event )
    {
        return;
    }
    // @}
    
    //! renvoie le tableau d'etat des touches du clavier.
    /*! permet de connaitre l'etat d'une touche. 
            le code ascii ou l'identifiant SDLK_xxx peuvent etre utilises comme indices dans le tableau. \n
            la liste des codes est dans <a href="http://www710.univ-lyon1.fr/~jciehl/Public/SDL_PG/sdlkey.html">libSDL Programming Guide</a> 

            exemple : getKeys()['a'], ou getKeys()[SDLK_UP]
     */
    unsigned char *getKeys( );

    //! renvoie l'etat d'une touche selon son code ascii ou son identifiant SDLK_xxx. 
    //! la liste des codes est dans <a href="http://www710.univ-lyon1.fr/~jciehl/Public/SDL_PG/sdlkey.html">libSDL Programming Guide</a> 
    /*! exemple : key('a'), ou key(SDLK_UP)
    
        la fonction renvoie une reference, on peut l'utiliser pour desactiver manuellement 
        une touche lorsque l'action a ete traitee et que l'on ne souhaite pas la re-executer a chaque image.
    
        \code
if(key('a')) {
    printf("a\n"); 
    key('a')= 0; 
}
        \endcode
     */
    unsigned char& key( const int key );

    //! \name encapsulation de la boucle principale
    //! deriver de App pour construire une nouvelle application.

    // @{
    //! appele apres l'initialisation complete de App et la creation du device.
    //! a redefinir en derivant App.
    //! doit renvoyer -1 en cas d'echec. 
    virtual int init( ) { return 0; }

    //! demarre la boucle principale.
    //! App fournit une version complete, il n'est pas necessaire de la redefinir.
    virtual int run( );

    //! appele avant la fermeture de l'application.
    //! a redefinir en derivant App.
    //! doit renvoyer -1 en cas d'echec. 
    virtual int quit( ) { return 0; }

    //! appele regulierement pour mettre a jour la scene (animations, deplacements, etc.).
    //! \param time millisecondes ecoulees depuis le lancement,
    //! \param delta millisecondes ecoulees depuis l'appel precedent.
    //! a redefinir en derivant App.
    //! doit renvoyer 0 pour sortir de la boucle principale, ou 1 pour continuer.
    virtual int update( const int time, const int delta ) { return 1; }
    
    //! appele regulierement pour afficher la scene.
    //! a redefinir en derivant App.
    //! doit renvoyer 0 pour sortir de la boucle principale, ou 1 pour continuer.
    virtual int draw( ) { return 1; }
    // @}
};

}

#endif
