
#ifndef _GK_ORBITER_H
#define _GK_ORBITER_H

#include "Transform.h"


namespace gk {

//! place et oriente un objet, construit une transformation XYZT.    
class Orbiter
{
protected:
    Transform m_transform;

    Point m_position;
    float m_rotation_x;
    float m_rotation_y;
    float m_rotation_z;

    //! renvoie la position de l'origine.
    const Point origin( ) const
    {
        return m_transform.inverse( Point(0.f, 0.f, 0.f) );
    }
    
    //! renvoie le vecteur 'vers le haut'.
    const Vector up( ) const // Y
    {
        return m_transform.inverse( Vector(0.f, 1.f, 0.f) );
    }
    
    //! renvoie le vecteur 'vers la droite'.
    const Vector right( ) const      // X
    {
        return m_transform.inverse( Vector(1.f, 0.f, 0.f) );
    }
    
    //! renvoie le vecteur 'devant'.
    const Vector forward( ) const    // Z
    {
        return m_transform.inverse( Vector(0.f, 0.f, 1.f) );
    }

public:
    //! constructeur par defaut : transformation identite.
    Orbiter( ) 
        :
        m_transform(),
        m_position(),
        m_rotation_x(0.f),
        m_rotation_y(0.f),
        m_rotation_z(0.f)
    {}
    
    //! constructeur : precise l'orientation de l'objet (et eventuellement sa position).
    Orbiter( const float rx, const float ry, const float rz, const Point& position= Point(0.f, 0.f, 0.f) ) 
        :
        m_transform(),
        m_position(position),
        m_rotation_x(rx),
        m_rotation_y(ry),
        m_rotation_z(rz)
    {
        transform();
    }
    
    //! destructeur.
    ~Orbiter( ) {}
    
    //! deplace l'objet, vers l'avant, +Z.
    void moveForward( const float v )
    {
        m_position= m_position + forward() * v;
    }
    
    //! deplace l'objet, vers la droite, +X.
    void moveRight( const float v )
    {
        m_position= m_position + right() * v;
    }
    
    //! deplace l'objet, en haut, +Y.
    void moveUp( const float v )
    {
        m_position= m_position + up() * v;
    }
    
    //! repositionne l'objet.
    void move( const Point& p )
    {
        m_position= p;
    }
    
    //! rotation gauche-droite (autour de up / Y local).
    void rotateUp( const float v )
    {
        m_rotation_y+= v;
    }
    
    //! rotation haut-bas (autour de right / X local).
    void rotateRight( const float v )
    {
        m_rotation_x+= v;
    }
    
    //! rotation autour de l'axe optique (forward / Z).
    void rotateForward( const float v )
    {
        m_rotation_z+= v;
    }
    
    //! renvoie la matrice de transformation.
    const Matrix4x4& matrix( )
    {
        return transform().matrix();
    }
    
    //! renvoie la transformation.
    const Transform& transform( )
    {
        m_transform= Translate( Vector(m_position) ) * RotateZ(m_rotation_z) * RotateX(m_rotation_x) * RotateY(m_rotation_y);
        return m_transform;
    }
};

}

#endif
