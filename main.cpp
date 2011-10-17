#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "Geometry.h"
#include "Transform.h"

#include "Triangle.h"
#include "PNTriangle.h"

#include "MeshIO.h"
#include "ImageIO.h"

#include "gkit/Camera.h"

using namespace std;

void drawCross( gk::Image * image, gk::Point p, gk::Pixel color ) {
    image->setPixel( p[0]-1, p[1], color );
    image->setPixel( p[0]+1, p[1], color );
    image->setPixel( p[0], p[1], color );
    image->setPixel( p[0], p[1]-1, color );
    image->setPixel( p[0], p[1]+1, color );
}

gk::Point P_Aitkem( float t, int i, int r, gk::Point * t_Point, float * t_t, int n ) {
    if ( r == 0 ) {
        return t_Point[r*n+i];
    }
    else {
        gk::Point A = ((t_t[i+r]-t)/(t_t[i+r]-t_t[i]))*P_Aitkem(t,i,r-1,t_Point,t_t,n);
        gk::Point B = ((t-t_t[i])/(t_t[i+r]-t_t[i]))*P_Aitkem(t,i+1,r-1,t_Point,t_t,n);
        t_Point[r*n+i] = A + B;
        return t_Point[r*n+i];
    }
}

void aitkem( gk::Image * image, int i, int r, gk::Point * t_Point, float * t_t, int n ) {
    //pas à pas
    for ( float j=t_t[0]; j<t_t[n-1]; j+=0.01 ) {
            gk::Point C = P_Aitkem( j, 0, r-1, t_Point, t_t, n );
            image->setPixel( C[0], C[1], gk::Pixel(0,255,0) );
    }
}

gk::Point P_DeCasteljau( float u, float v, gk::Point * t_Point, int n ) {
    int n_2 = n*n;
    for ( int r=1; r<n; r++ ) {
        for ( int i=0; i<n-r; i++ ) {
            for ( int j=0; j<n-r; j++ ) {
                t_Point[r*n_2 + i*n + j] = (1-u)*(1-v)*t_Point[(r-1)*n_2 + i*n +j] + (1-u)*v*t_Point[(r-1)*n_2 + i*n + (j+1)]
                                            + u*(1-v)*t_Point[(r-1)*n_2 + (i+1)*n + j] + u*v*t_Point[(r-1)*n_2 + (i+1)*n + (j+1)];
            }
        }
    }

    return t_Point[(n-1)*n*n];
}

void DeCasteljau( gk::Point * t_Maillage, int nb_pas, gk::Point * t_Point, int n ) {
    for ( int u=1; u<=nb_pas; u++ ) {
        for ( int v=1; v<=nb_pas; v++ ) {
            gk::Point p = P_DeCasteljau( ((float) u/nb_pas), ((float) v/nb_pas), t_Point, n );
            t_Maillage[(u-1)*nb_pas + (v-1)] = p;
        }
    }
}

string pointToObj( gk::Point p ) {
    ostringstream oss;
    oss << "v " << p[0] << " " << p[1] << " " << p[2] << endl;
    string s = oss.str();

    return s;
}

int main( int argc, char ** argv )
{
	// creer une image resultat
	gk::Image *image= new gk::Image(512, 512);

	gk::Transform model;    // transformation identité
	gk::Transform projection= gk::Perspective(50.f, (float) image->width() / (float) image->height(), 1.f, 1000.f);  // projection perspective
	gk::Transform viewport= gk::Viewport(image->width(), image->height());      // transformation adaptee a la resolution de l'image resultat

    gk::FirstPersonCamera cam = gk::FirstPersonCamera();
    cam.setProjection(projection);
    cam.setViewport(viewport);

	// compose les transformations, sauf viewport : produit dans le sens inverse des changements de reperes representes ...
	gk::Transform mvp= cam.projectionTransform() * cam.viewportTransform() * model;

    gk::Point p00(1,3,-20);
    gk::Point p01(3,3,-20);
    gk::Point p02(3,1,-20);

    gk::Point p10(1,3,-40);
    gk::Point p11(3,3,-40);
    gk::Point p12(3,1,-40);

    gk::Point p20(1,3,-60);
    gk::Point p21(3,3,-60);
    gk::Point p22(3,1,-60);

/*    pts[0][0] = gk::Point(100,100,-100);*/
/*    pts[0][1] = gk::Point(110,160,-100);*/
/*    pts[0][2] = gk::Point(120,220,-100);*/
/*    pts[0][3] = gk::Point(125,280,-100);*/
/**/
/*    pts[1][0] = gk::Point(200,90,-100);*/
/*    pts[1][1] = gk::Point(210,160,-100);*/
/*    pts[1][2] = gk::Point(220,216,-100);*/
/*    pts[1][3] = gk::Point(230,270,-100);*/
/**/
/*    pts[2][0] = gk::Point(300,95,-100);*/
/*    pts[2][1] = gk::Point(305,170,-100);*/
/*    pts[2][2] = gk::Point(310,215,-100);*/
/*    pts[2][3] = gk::Point(315,280,-100);*/
/**/
/*    pts[3][0] = gk::Point(420,87,-100);*/
/*    pts[3][1] = gk::Point(440,162,-100);*/
/*    pts[3][2] = gk::Point(425,207,-100);*/
/*    pts[3][3] = gk::Point(422,272,-100);*/

/*    int n=3;*/
    int n=4;

    int nb_pas = 100;

    gk::Point * t_Maillage = (gk::Point *) malloc(sizeof(gk::Point)*nb_pas*nb_pas);
    gk::Point * t_Point = (gk::Point *) malloc(sizeof(gk::Point)*n*n*n);
/*    t_Point[0] = p00;*/
/*    t_Point[1] = p01;*/
/*    t_Point[2] = p02;*/
/**/
/*    t_Point[1*n+0] = p10;*/
/*    t_Point[1*n+1] = p11;*/
/*    t_Point[1*n+2] = p12;*/
/**/
/*    t_Point[2*n+0] = p20;*/
/*    t_Point[2*n+1] = p21;*/
/*    t_Point[2*n+2] = p22;*/

    t_Point[0*n+0] = gk::Point(1,1,-20);
    t_Point[0*n+1] = gk::Point(1.1,1.6,-20);
    t_Point[0*n+2] = gk::Point(1.2,2.2,-20);
    t_Point[0*n+3] = gk::Point(1.25,2.8,-20);

    t_Point[1*n+0] = gk::Point(2,0.9,-20);
    t_Point[1*n+1] = gk::Point(2.1,1.6,-20);
    t_Point[1*n+2] = gk::Point(2.2,2.16,-20);
    t_Point[1*n+3] = gk::Point(2.3,2.7,-20);

    t_Point[2*n+0] = gk::Point(3,0.95,-20);
    t_Point[2*n+1] = gk::Point(3.05,1.7,-20);
    t_Point[2*n+2] = gk::Point(3.1,2.15,-20);
    t_Point[2*n+3] = gk::Point(3.15,2.8,-20);

    t_Point[3*n+0] = gk::Point(4.2,0.87,-20);
    t_Point[3*n+1] = gk::Point(4.4,1.62,-20);
    t_Point[3*n+2] = gk::Point(4.25,2.07,-20);
    t_Point[3*n+3] = gk::Point(4.22,2.72,-20);

    for ( int i=0; i<n; i++ ) {
        for ( int j=0; j<n; j++ ) {
            gk::Point p = t_Point[i*n+j];
            gk::HPoint ph;
            mvp(p, ph);

            if(ph.isVisible()) {
                gk::Point q = viewport(ph.project());
                image->setPixel( q.x, q.y, gk::Pixel(255, 0, 0) );
            }
        }
    }

    DeCasteljau( t_Maillage, nb_pas, t_Point, n );

    string exp = "g surface\n";
/*    exp.append(pointToObj( p0 ));*/
/*    exp.append(pointToObj( p1 ));*/
/*    exp.append(pointToObj( p2 ));*/
/*    exp.append("f 1 2 3\n");*/
/**/
    ofstream file("export.obj");
/*    if ( !file.fail() ) {*/
/*        file << exp.c_str() << endl;*/
/*        file.close();*/
/*    }*/

    for ( int i=0; i<nb_pas; i++ ) {
        if ( !file.fail() ) {
            file << "f";
            for ( int j=0; j<nb_pas; j++ ) {
                file << " " << (i*nb_pas+j+1);
            }
            file << endl;
        }
        for ( int j=0; j<nb_pas; j++ ) {
            gk::Point p = t_Maillage[i*nb_pas + j];

/*            cout << "x y z : " << p[0] << " " << p[1] << " " << p[2] << endl;*/

            gk::HPoint ph;
            mvp(p, ph);

            //gk::Point q = ph.project();
            if(ph.isVisible()) {
                gk::Point q = viewport(ph.project());
/*                printf("%f %f %f\n", q.x, q.y, q.z);*/
                image->setPixel( q.x, q.y, gk::Pixel(255, 255, 255) );
/*                cout << "x y : " << p[0] << " " << p[1] << endl;*/
            }

            if ( !file.fail() ) {
                string s = pointToObj( p );
                file << s.c_str();
            }

        }
    }

    if ( !file.fail() ) {
        file.close();
    }

    free(t_Maillage);
    free(t_Point);

	// enregistrer l'image
	gk::ImageIO::write(image, "output.bmp");
	delete image;

	// pas la peine de detruire l'objet mesh, gk::MeshIO le fait automatiquement.
	return EXIT_SUCCESS;
}
