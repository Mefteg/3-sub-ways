
#ifndef _GK_TRIANGLE_H
#define _GK_TRIANGLE_H

#include "Geometry.h"
#include "Transform.h"
#include "Sampler.h"

namespace gk {
    
//! representation d'un triangle 'geometrique'.
struct Triangle
{
    Point a, b, c;
    float area;
    
    //! constructeur par defaut.
    Triangle( ) {}
    
    //! construit un triangle connaissant ses 3 sommets.
    Triangle( const Point& a, const Point& b, const Point& c )
        :
        a(a),  b(b), c(c),
        area(getArea())
    {}
    
    //! destructeur.
    ~Triangle( ) {}
    
    float getArea( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        return .5f * Cross(ab, ac).Length();
    }
    
    Normal getNormal( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        return Normal( Normalize(Cross(ab, ac)) );
    }
    
    BBox getBBox( ) const
    {
        BBox bbox;
        bbox.Union(a);
        bbox.Union(b);
        bbox.Union(c);
        
        return bbox;
    }
    
    //! renvoie un triangle transforme par 't'.
    Triangle transform( const Transform& t )
    {
        return Triangle( t(a), t(b), t(c) );
    }
    
    //! intersection avec un rayon.
    //! renvoie faux s'il n'y a pas d'intersection, une intersection peut exister mais peut ne pas se trouver dans [tmin tmax] du rayon.
    //! renvoie vrai + les coordonnees barycentriques du point d'intersection + sa position le long du rayon.
    //! convention barycentrique : t(u, v)= (1 - u - v) * a + u * b + v * c
    //! utiliser Mesh::getUVNormal() et Mesh::getUVTexCoord() pour interpoler les attributs du point d'intersection.
    bool Intersect( const Ray &ray, const float htmax, 
        float &rt, float &ru, float&rv ) const
    {
        /* begin calculating determinant - also used to calculate U parameter */
        Vector ac(a, c);
        const Vector pvec= Cross(ray.d, ac);
        
        /* if determinant is near zero, ray lies in plane of triangle */
        Vector ab(a, b);
        const float det= Dot(ab, pvec);
        if (det > -EPSILON && det < EPSILON)
            return false;
        
        const float inv_det= 1.0f / det;
        
        /* calculate distance from vert0 to ray origin */
        const Vector tvec(a, ray.o);
        
        /* calculate U parameter and test bounds */
        const float u= Dot(tvec, pvec) * inv_det;
        if(u < 0.0f || u > 1.0f)
            return false;
        
        /* prepare to test V parameter */
        const Vector qvec= Cross(tvec, ab);
        
        /* calculate V parameter and test bounds */
        const float v= Dot(ray.d, qvec) * inv_det;
        if(v < 0.0f || u + v > 1.0f)
            return false;
        
        /* calculate t, ray intersects triangle */
        rt= Dot(ac, qvec) * inv_det;
        ru= u;
        rv= v;
        
        // ne renvoie vrai que si l'intersection est valide (comprise entre tmin et tmax du rayon)
        return (rt < htmax && rt > ray.tmin);
    }
    
    //! renvoie un point a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Point getUVPoint( const float u, const float v ) const
    {
        const float w= 1.f - u - v;
        return a * w + b * u + c * v;
    }

    //! choisit un point aleatoirement a la surface du triangle et renvoie la probabilite de l'avoir choisi.
    float sampleUniform( Sampler& sampler, Point& p ) const
    {
        float s= sqrtf(sampler.uniformFloat());
        float t= sampler.uniformFloat();
        float u= 1.f - s;
        float v= (1.f - t) * s;
        float w= t * s;
        
        p= Point(u * a + v * b + w * c);
        return 1.f / area;
    }
    
    //! renvoie la probabilite de choisir le point p aleatoirement.
    //! remarque: le point doit appartenir au triangle.
    float pdfUniform( const Point& p ) const
    {
        return 1.f / area;
    }
};

}
#endif
