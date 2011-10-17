
#ifndef _IOEFFECT_H
#define _IOEFFECT_H

#include "IOManager.h"
#include "Effect.h"


namespace gk {

//! manager pour importer des effets / scripts.
class EffectIO : public IOManager<Effect>
{
    // non copyable
    EffectIO( const EffectIO& );
    EffectIO& operator=( const EffectIO& );
    
    // private default constructor, singleton
    EffectIO( )
        :
        IOManager<Effect>( )
    {}
    
public:
    //! importe l'objet 'name' du script 'filename'
    static
    Effect *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        Effect *effect= manager().find(filename, name);
        if(effect != NULL)
            return effect;
        
        // importer le fichier
        effect= new Effect;
        if(effect->read(filename) < 0)
        {
            delete effect;
            return NULL;
        }
        
        // reference le script avec le manager
        return manager().insert(effect, filename, name);
    }

    static
    EffectIO& manager( )  // singleton
    {
        static EffectIO manager;
        return manager;
    }
};

}

#endif
