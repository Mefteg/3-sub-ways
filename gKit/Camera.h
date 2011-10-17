
#ifndef _GK_CAMERA_H
#define _GK_CAMERA_H

#include "Transform.h"


namespace gk {

//! camera generique.
class Camera
{
protected:
    Transform m_view;
    Transform m_projection;
    Transform m_viewport;

    float m_width;
    float m_height;
    
    //! renvoie la position de l'origine.
    const Point origin( ) const
    {
        return m_view.inverse( Point(0.f, 0.f, 0.f) );
    }
    
    //! renvoie le vecteur 'vers le haut'.
    const Vector up( ) const // Y
    {
        return m_view.inverse( Vector(0.f, 1.f, 0.f) );
    }
    
    //! renvoie le vecteur 'vers la droite'.
    const Vector right( ) const      // X
    {
        return m_view.inverse( Vector(1.f, 0.f, 0.f) );
    }
    
    //! renvoie le vecteur 'devant'.
    const Vector forward( ) const    // Z
    {
        return m_view.inverse( Vector(0.f, 0.f, 1.f) );
    }

public:
    //! constructeur par defaut.
    Camera( )
        :
        m_view(),
        m_projection(),
        m_viewport( Viewport(100.f, 100.f) ),
        m_width(100.f),
        m_height(100.f)
    {}
    
    //! constructeur, definit la projection utilisee par la camera.
    Camera( const Transform& projection, const int width= 100, const int height= 100 )
        :
        m_view(),
        m_projection(projection),
        m_viewport( Viewport(width, height) ),
        m_width(width),
        m_height(height)
    {}
    
    //! destructeur.
    ~Camera() {}
    
    //! modifie le viewport de la camera.
    void setViewport( const int width, const int height )
    {
        m_width= width;
        m_height= height;
        m_viewport= Viewport(width, height);
    }
    
    //! modifie le viewport de la camera.
    void setViewport( int viewport[4] )
    {
        m_width= viewport[2];
        m_height= viewport[3];
        m_viewport= Viewport(viewport[2], viewport[3]);
    }
    
    //! modifie la projection de la camera.
    void setProjection( const Transform& projection )
    {
        m_projection= projection;
    }
    
    //! renvoie la matrice de projection de la camera.
    const Matrix4x4& projection( )
    {
        return m_projection.matrix();
    }
    
    //! renvoie la transformation de projection de la camera.
    const Transform& projectionTransform( )
    {
        return m_projection;
    }

    //! renvoie la matrice view, changement de repere de la camera.
    const Matrix4x4& view( )
    {
        return m_view.matrix();
    }
    
    //! renvoie la transforamtion view de la camera.
    const Transform& viewTransform( )
    {
        return m_view;
    }
    
    //! renvoie la matrice viewport
    const Matrix4x4& viewport( )
    {
        return m_viewport.matrix();
    }
    
    //! renvoie la transformation viewport
    const Transform& viewportTransform( )
    {
        return m_viewport;
    }
};

//! camera perspective.
class PerspectiveCamera : public gk::Camera
{
public:
    //! constructeur par defaut.
    PerspectiveCamera( )
        :
        gk::Camera()
    {}
    
    //! constructeur, parametre la projection perspective, cf gk::Perspective().
    PerspectiveCamera( const float fov, const float aspect, const float znear, const float zfar, const int width= 100, const int height= 100 )
        :
        Camera( Perspective(fov, aspect, znear, zfar), width, height)
    {}
    
    //! destructeur.
    ~PerspectiveCamera( ) {}
};

//! camera first person.
class FirstPersonCamera : public PerspectiveCamera
{
protected:
    Point m_position;
    float m_rotation_x;
    float m_rotation_y;
    float m_rotation_z;
    
public:
    FirstPersonCamera( )
        :
        PerspectiveCamera(),
        m_position(),
        m_rotation_x(0.f),
        m_rotation_y(0.f),
        m_rotation_z(0.f)
    {}

    //! constructeur, parametre la projection perspective, cf gk::Perspective().
    FirstPersonCamera( const float fov, const float aspect, const float znear, const float zfar, const int width= 100, const int height= 100 )
        :
        PerspectiveCamera(fov, aspect, znear, zfar, width, height),
        m_position(),
        m_rotation_x(0.f),
        m_rotation_y(0.f),
        m_rotation_z(0.f)
    {}
    
    ~FirstPersonCamera( ) {}
    
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
    
    //! renvoie la 'position' de la camera.
    const Point& position( ) 
    {
        return m_position;
    }
    
    //! renvoie la matrice view, changement de repere de la camera.
    const Matrix4x4& view( )
    {
        return viewTransform().matrix();
    }
    
    //! renvoie la transforamtion view de la camera.
    const Transform& viewTransform( )
    {
        m_view= (Translate( Vector(m_position) ) * RotateZ(m_rotation_z) * RotateX(m_rotation_x) * RotateY(m_rotation_y)).getInverse();
        return m_view;
    }    
};


//! camera orthographique.
class OrthographicCamera : public gk::Camera
{
public:
    //! constructeur par defaut.
    OrthographicCamera( )
        :
        gk::Camera()
    {}
    
    //! constructeur, parametre la projection orthographique, cf gk::Orthographic().
    OrthographicCamera( const float znear, const float zfar, const int width= 100, const int height= 100 )
        :
        Camera( Orthographic(znear, zfar), width, height )
    {}
    
    //! destructeur.
    ~OrthographicCamera( ) {}
};

}

#endif
