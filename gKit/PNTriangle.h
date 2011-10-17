
#ifndef _GK_PN_TRIANGLE_H
#define _GK_PN_TRIANGLE_H

#include "Geometry.h"
#include "Transform.h"
#include "Triangle.h"

namespace gk {
    
//! representation d'un triangle 'geometrique'.
struct PNTriangle
{
    Point a, b, c;
    Normal na, nb, nc;
    
    //! constructeur par defaut.
    PNTriangle( ) {}
    
    //! construit un triangle connaissant ses 3 sommets et leurs normales.
    PNTriangle( 
        const Point& _a, const Normal& _na, 
        const Point& _b, const Normal& _nb,
        const Point& _c, const Normal& _nc )
        :
        a(_a),  b(_b), c(_c),
        na(_na), nb(_nb), nc(_nc)
    {}
    
    //! destructeur.
    ~PNTriangle( ) {}
    
    //! renvoie l'aire du triangle a, b, c.
    float getBaseArea( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        return .5f * Cross(ab, ac).Length();
    }
    
    //! renvoie la normale geometrique du triangkle a, b, c.
    Normal getBaseNormal( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        return Normal( Normalize(Cross(ab, ac)) );
    }
    
    //! renvoie la bbox du triangle a, b, c.
    BBox getBaseBBox( ) const
    {
        BBox bbox;
        bbox.Union(a);
        bbox.Union(b);
        bbox.Union(c);
        
        return bbox;
    }
    
    //! renvoie un pntriangle transforme par 't'.
    PNTriangle transform( const Transform& t )
    {
        return PNTriangle( t(a), t(na), t(b), t(nb), t(c), t(nc) );
    }
    
    //! renvoie le triangle geoemtrique
    Triangle getBaseTriangle( )
    {
        return Triangle(a, b, c);
    }
};

}
#endif
