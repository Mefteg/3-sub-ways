
#ifndef _SAMPLER_H
#define _SAMPLER_H

#include <cstdlib>
#include <time.h>

namespace gk {
    
//! representation d'un generateur aleatoire ... juste pour rire
class Sampler
{
public:
    Sampler( ) {}
    
    float uniformFloat( )
    {
        return (float) rand() / (float) RAND_MAX;
    }
    
    int uniformInt( const int max )
    {
        return ((float) rand() * (float) max / (float) RAND_MAX) + .5f;
    }
    
    //! initialisation du germe du generateur aleatoire.
    static void init( const int seed= 0 )
    {
        if(seed != 0)
            srand(seed);
        else
            srand(time(NULL));
    }
};

}

#endif
