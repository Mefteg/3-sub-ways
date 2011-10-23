
// basic image viewer loading hdr + sdl_image supported formats : png, jpg, bmp, tga images.

#include "nvwidgets/nvSdlWidgets.h"
#include "App.h"

#include "GL/GLPlatform.h"
#include "GLManager.h"
#include "EffectIO.h"
#include "EffectShaderManager.h"
#include "ImageIO.h"
#include "TextureManager.h"
#include "SamplerManager.h"
#include "GL/TPSampler.h"
#include "GL/TPTextureUnits.h"
#include "GL/TPShaderProgram.h"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "utils.h"

#include "Geometry.h"
#include "Transform.h"

#include "Triangle.h"
#include "PNTriangle.h"

#include "MeshIO.h"
#include "ImageIO.h"

#include "Camera.h"

#include "gKit/face.h"
#include "gKit/vertex.h"
#include "gKit/halfedge.h"

using namespace std;

class ImageViewer : public gk::App
{
    std::string m_filename;
    gk::GLTexture2D *m_image;
    gk::GLTexture2D *m_colors;
    gk::GLShaderProgram *m_program;
    gk::GLSampler *m_sampler;

    nv::SdlUIContext m_ui;

    float m_compression;
    float m_saturation_max;
    float m_saturation_delta;

    float m_saturation;
    float m_saturation_fine;
    int m_mode;
    bool m_show_histogram;

    float m_ymin;
    float m_ymax;

    float m_bins[256];
    int m_bins_n;
    float m_bins_min;
    float m_bins_max;

    public:
    ImageViewer( const int w, const int h, const std::string& filename )
        :
            gk::App(w, h),
            m_filename(filename),
            m_image(NULL),
            m_colors(NULL),
            m_program(NULL),
            m_sampler(NULL),
            m_compression(2.f),
            m_saturation_max(1.f),
            m_saturation_delta(.1f),
            m_saturation(1.f),
            m_saturation_fine(0.f),
            m_mode(0),
            m_show_histogram(true),
            m_ymin(0.f),
            m_ymax(1.f),
            m_bins_n(256),
            m_bins_min(0.f),
            m_bins_max(1.f)
    {
        m_ui.init(w, h);
    }

    ~ImageViewer( ) {}

    void processWindowResize( SDL_ResizeEvent& event )
    {
        m_ui.reshape(event.w, event.h);
    }

    void processMouseButtonEvent( SDL_MouseButtonEvent& event )
    {
        m_ui.processMouseButtonEvent(event);
    }

    void processMouseMotionEvent( SDL_MouseMotionEvent& event )
    {
        m_ui.processMouseMotionEvent(event);
    }

    void processKeyboardEvent( SDL_KeyboardEvent& event )
    {
        m_ui.processKeyboardEvent(event);
    }

    int init( )
    {
        gk::TextureUnitState::init();

        // charge la texture de fausses couleurs
        gk::Image *colors= gk::ImageIO::read("false_colors.png");
        if(colors == NULL)
            return -1;
        m_colors= gk::createTexture2D(gk::UNIT0, colors);
        if(m_colors == NULL || m_colors->createGLResource() < 0)
            return -1;

        m_sampler= gk::createSampler();
        if(m_sampler == NULL || m_sampler->createGLResource() < 0)
            return -1;
        //~ m_sampler->setWrapMode(GL_CLAMP, GL_CLAMP);

        // charge le shader de 'tone' mapping hdr
        gk::Effect *effect= gk::EffectIO::read("hdr_tone.gkfx");
        m_program= gk::EffectShaderManager(effect).createShaderProgram("hdr");
        if(m_program == NULL || m_program->createGLResource() < 0)
            return -1;

        // charge l'image
        gk::HDRImage *hdr= gk::HDRImageIO::read(m_filename);
        if(hdr == NULL)
        {
            printf(" -- '%s' failed.\n", m_filename.c_str());
            return -1;
        }

        m_image= gk::createTexture2D(gk::UNIT0, hdr);
        if(m_image == NULL || m_image->createGLResource() < 0)
            return -1;

        // modifier le titre de la fenetre
        SDL_WM_SetCaption(m_filename.c_str(), "");

        // evaluer les valeurs initiales de compression et saturation
        // . valeurs par defaut
        m_compression= 2.f;
        m_saturation= 100.f;
        m_saturation_max= 100.f;
        m_saturation_delta= 10.f;
        m_saturation_fine= 0.f;
        m_mode= 0.f;

        m_bins_n= 256;
        for(int i= 0; i< m_bins_n; i++)
            m_bins[i]= 0.f;

        // . trouver les extremess
        float ymin= HUGE_VAL;
        float ymax= -HUGE_VAL;
        float sum= 0.f;
        for(int y= 0; y < hdr->height(); y++)
            for(int x= 0; x < hdr->width(); x++)
            {
                const gk::HDRPixel &pixel= hdr->getPixel(x, y);
                const float l= (pixel.r + pixel.g + pixel.b) / 3.f;

                if(l < ymin)
                    ymin= l;
                if(l > ymax)
                    ymax= l;

                sum= sum + l;
            }

        m_ymin= ymin;
        m_ymax= ymax;

        printf("min %f < %f < max %f\n",
                ymin, sum / (hdr->width() * hdr->height()), ymax);

        // . construit l'histogramme
        const float bin_scale= (float) m_bins_n / (ymax - ymin);
        for(int y= 0; y < hdr->height(); y++)
            for(int x= 0; x < hdr->width(); x++)
            {
                const gk::HDRPixel &pixel= hdr->getPixel(x, y);
                const float l= (pixel.r + pixel.g + pixel.b) / 3.f;

                int k= (l - ymin) * bin_scale;
                if(k < 0)
                    k= 0;
                if(k >= m_bins_n)
                    k= m_bins_n -1;

                m_bins[k]+= 1.f;
            }

        m_bins_min= hdr->width() * hdr->height();
        m_bins_max= 0;
        float bins_sum= 0.f;
        const float bin_normalize= 1.f / (hdr->width() * hdr->height());
        for(int i= 0; i < m_bins_n; i++)
        {
            //~ printf("%d: %d\n", i, (int) m_bins[i]);
            m_bins[i]= m_bins[i] * bin_normalize;

            if(m_bins[i] < m_bins_min)
                m_bins_min= m_bins[i];
            if(m_bins[i] > m_bins_max)
                m_bins_max= m_bins[i];

            bins_sum+= m_bins[i];
        }

        // . reglage de la dynamique de l'historgramme
        m_bins_max= bins_sum / m_bins_n;

        // . reglage des plages des sliders
        m_saturation= ymax;
        m_saturation_max= ymax;
        m_saturation_delta= ymax / 10.f;
        m_saturation_fine= 0.f;
        m_mode= 0.f;
        return 0;
    }

    int draw( )
    {
        if(key(SDLK_ESCAPE))
            Close();

        if(key('n'))
            resizeWindow(m_image->width(), m_image->height());

        if(key(SDLK_SPACE))
        {
            m_show_histogram= !m_show_histogram;
            key(SDLK_SPACE)= 0;
        }

        const float x= 0.f;
        const float y= 0.f;
        const float z= -.5f;
        const float w= m_image->width();
        const float h= m_image->height();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0., w, 0., h, -1., 1.);

        glViewport(0, 0, windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gk::setShaderProgram(m_program);
        gk::setUniform(m_program->uniform("compression"), m_compression);
        gk::setUniform(m_program->uniform("saturation"), m_saturation + m_saturation_fine);

        if(m_mode == 1)
            gk::setUniform(m_program->uniform("heat"), 1.f);
        else
            gk::setUniform(m_program->uniform("heat"), 0.f);

        gk::setTextureUnit(m_program->sampler("image"), m_image, m_sampler);
        gk::setTextureUnit(m_program->sampler("colors"), m_colors, m_sampler);

        glColor3f(1.f, 0.f, 1.f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(x, y+h, z);

        glTexCoord2f(0.f, 0.f);
        glVertex3f(x, y, z);

        glTexCoord2f(1.f, 0.f);
        glVertex3f(x+w, y, z);

        glTexCoord2f(1.f, 1.f);
        glVertex3f(x+w, y+h, z);
        glEnd();

        gk::resetTextureUnit(m_program->sampler("image"));
        gk::resetTextureUnit(m_program->sampler("colors"));
        gk::resetShaderProgram();

        if(m_show_histogram)
        {
            glColor3f(1.f, 0.f, 0.f);
            glLineWidth(2.f);
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINES);
            for(int i= 0; i < m_bins_n; i++)
            {
                glVertex2f(10.f + (float) i / (float) m_bins_n * (float) (w / 2), 10.f);
                glVertex2f(10.f + (float) i / (float) m_bins_n * (float) (w / 2), 10.f + m_bins[i] / m_bins_max * (float) h / 2.f);
            }
            glEnd();

            // visualise compression dans l'histogramme
            float bin_scale= (float) m_bins_n / (m_ymax - m_ymin);
            float saturation_height= (float) h;

            int saturation_bin= (m_saturation + m_saturation_fine - m_ymin) * bin_scale;
            glColor3f(1.f, 1.f, 0.f);
            glBegin(GL_LINES);
            glVertex2f(10.f + (float) saturation_bin / (float) m_bins_n * (float) (w / 2), 10.f);
            glVertex2f(10.f + (float) saturation_bin / (float) m_bins_n * (float) (w / 2), 10.f + saturation_height);
            glEnd();

            // trace la courbe de compression
            glColor3f(0.f, 0.f, 1.f);
            glBegin(GL_LINES);
            for(int i= 0; i < m_bins_n; i++)
            {
                float k1= 1.0 / powf(m_saturation + m_saturation_fine, 1.f / m_compression); // normalisation : saturation == blanc
                float y= (float) (i+1) / (float) m_bins_n * (m_ymax - m_ymin) + m_ymin;
                float color= y * k1 * powf(y, 1.f / m_compression);

                glVertex2f(10.f + (float) i / (float) m_bins_n * (float) (w / 2), 10.f + color * h/2 );
            }
            glEnd();
        }

        m_ui.begin();
        m_ui.beginGroup(nv::GroupFlags_GrowDownFromLeft);
        m_ui.beginGroup(nv::GroupFlags_GrowRightFromTop);
        m_ui.doLabel(nv::Rect(), "saturation");
        {
            char tmp[1024];
            sprintf(tmp, "%.6f", m_saturation + m_saturation_fine);

            int edit= 0;
            if(m_ui.doLineEdit(nv::Rect(), tmp, sizeof(tmp), &edit))
            {
                float value= m_saturation + m_saturation_fine;
                if(sscanf(tmp, "%f", &value) == 1)
                {
                    m_saturation= value;
                    m_saturation_fine= 0.f;
                }
            }
        }

        if(m_ui.doHorizontalSlider(nv::Rect(0,0, 200, 0), 0.f, m_saturation_max * 4.f, &m_saturation))
            m_saturation_fine= 0.f;

        m_ui.doHorizontalSlider(nv::Rect(), 0.f, m_saturation_delta, &m_saturation_fine);
        m_ui.doCheckButton(nv::Rect(), "histogram", &m_show_histogram);
        m_ui.endGroup();

        m_ui.beginGroup(nv::GroupFlags_GrowRightFromTop);
        m_ui.doRadioButton(1, nv::Rect(), "heat", &m_mode);
        m_ui.doRadioButton(0, nv::Rect(), "compression", &m_mode);

        if(m_mode == 0)
        {
            char tmp[1024];
            sprintf(tmp, "%.2f", m_compression);

            int edit= 0;
            if(m_ui.doLineEdit(nv::Rect(), tmp, sizeof(tmp), &edit))
            {
                float value= m_compression;
                if(sscanf(tmp, "%f", &value) == 1)
                    m_compression= value;
            }

            m_ui.doHorizontalSlider(nv::Rect(0,0, 200, 0), 0.f, 10.f, &m_compression);
        }
        m_ui.endGroup();
        m_ui.endGroup();
        m_ui.end();

        SDL_GL_SwapBuffers();
        return 1;
    }
};

void drawCross( gk::Image * image, gk::Point p, gk::Pixel color ) {
    image->setPixel( p[0]-1, p[1], color );
    image->setPixel( p[0]+1, p[1], color );
    image->setPixel( p[0], p[1], color );
    image->setPixel( p[0], p[1]-1, color );
    image->setPixel( p[0], p[1]+1, color );
}

Vertex * P_DeCasteljau( float u, float v, Vertex ** t_Vertex, int n ) {
    int n_2 = n*n;
    for ( int r=1; r<n; r++ ) {
        for ( int i=0; i<n-r; i++ ) {
            for ( int j=0; j<n-r; j++ ) {
                t_Vertex[r*n_2 + i*n + j] = new Vertex( (1-u)*(1-v)*t_Vertex[(r-1)*n_2 + i*n +j]->v + (1-u)*v*t_Vertex[(r-1)*n_2 + i*n + (j+1)]->v
                    + u*(1-v)*t_Vertex[(r-1)*n_2 + (i+1)*n + j]->v + u*v*t_Vertex[(r-1)*n_2 + (i+1)*n + (j+1)]->v );
            }
        }
    }

    return t_Vertex[(n-1)*n*n];
}

void DeCasteljau( Vertex ** t_Maillage, int nb_pas, Vertex ** t_Vertex, int n ) {
    for ( int u=1; u<=nb_pas; u++ ) {
        for ( int v=1; v<=nb_pas; v++ ) {
            Vertex * p = P_DeCasteljau( ((float) u/nb_pas), ((float) v/nb_pas), t_Vertex, n );
            t_Maillage[(u-1)*nb_pas + (v-1)] = p;
        }
    }
}

// !! ne fonctionne pas, probleme avec les aretes paires surement !!
void Loop( vector<Vertex *> * v_Vertex, vector<Halfedge *> * v_Halfedge, vector<Face *> * v_Face ) {
    Halfedge * h;
    int old_size = v_Halfedge->size();
    //pour chaque arete
    for ( int i=0; i<old_size; i++ ) {
        h = v_Halfedge->at(i);
        //si l'arete n'a pas deja été traitée par sa paire
        if ( h->done == false ) {
            //si l'arete n'a pas de paire
            if ( h->he_e == NULL ) {
                //on subdivide l'arete
                Halfedge * h2 = h->subdivise();
                //on ajoute le nouveau sommet généré par subdivise
                v_Vertex->push_back(h->v);
                //on ajoute la nouvelle arête générée par subdivise
                v_Halfedge->push_back(h2);
                h->done = true;
                h2->done = true;
            }
            //sinon
            else {
                Halfedge * h_e = h->he_e;
                //on subdivise les deux
                Halfedge * h2 = h->subdivise();
                Halfedge * h_e2 = h_e->subdivise();
                //parité
                h->he_e = h_e2;
                h_e2->he_e = h;
                h2->he_e = h_e;
                h_e->he_e = h2;
                v_Halfedge->push_back(h2);
                v_Halfedge->push_back(h_e2);
                //et on supprime l'un des sommets
                delete h_e->v;
                h_e->v = h->v;
                v_Vertex->push_back(h->v);
                h->done = true;
                h2->done = true;
                h_e->done = true;
                h_e2->done = true;
            }
        }
    }

    old_size = v_Face->size();
    //pour chaque face
    for ( int i=0; i<old_size; i++ ) {
        cout << "Face " << i << endl;
        //je la divise
        Face * f = v_Face->at(i);
        //pour chaque ancienne arete de la face
        Halfedge * h1 = f->he;
        Halfedge * h2 = h1->he_n->he_n;
        Halfedge * h3 = h2->he_n->he_n;
        //les nouvelles aretes
        Halfedge * ha = h1->he_n;
        Halfedge * hb = h2->he_n;
        Halfedge * hc = h3->he_n;

        //les nouvelles faces
        Face * f1 = new Face(h1);
        Face * f2 = new Face(h2);
        Face * f3 = new Face(h3);

        Halfedge * hf1 = new Halfedge(h3->v, hc, NULL, f1);
        h1->he_n = hf1;
        Halfedge * hf1e = new Halfedge(h1->v, NULL, hf1, f);
        hf1->he_e = hf1e;
        f->he = hf1e;

        Halfedge * hf2 = new Halfedge(h1->v, ha, NULL, f2);
        h2->he_n = hf2;
        Halfedge * hf2e = new Halfedge(h2->v, NULL, hf2, f);
        hf2->he_e = hf2e;

        Halfedge * hf3 = new Halfedge(h2->v, hb, NULL, f3);
        h3->he_n = hf3;
        Halfedge * hf3e = new Halfedge(h3->v, NULL, hf3, f);
        hf3->he_e = hf3e;

        hf1e->he_n = hf2e;
        hf2e->he_n = hf3e;
        hf3e->he_n = hf1e;

        h1->f = f1;
        hc->f = f1;
        h2->f = f2;
        ha->f = f2;
        h3->f = f3;
        hb->f = f3;

        v_Halfedge->push_back(hf1);
        v_Halfedge->push_back(hf1e);
        v_Halfedge->push_back(hf2);
        v_Halfedge->push_back(hf2e);
        v_Halfedge->push_back(hf3);
        v_Halfedge->push_back(hf3e);

        v_Face->push_back(f1);
        v_Face->push_back(f2);
        v_Face->push_back(f3);

        f1->print();
        cout << " ~~~ " << endl;
        f->print();
    }

    //pour chaque arete
    for ( int i=0; i<v_Halfedge->size(); i++ ) {
        Halfedge * h = v_Halfedge->at(i);

        //si son sommet extremité n'a pas été traité
        if ( !h->v->done ) {
            //si l'arete n'a pas de paire
            if ( h->he_e == NULL ) {

            }
        }
    }

    int size = v_Halfedge->size();
    for ( int i=0; i<size; i++ ) {
        h = v_Halfedge->at(i);
        h->done = false;
    }
}

void Butterfly(vector<Vertex *>* vertex, vector<Halfedge *>* halfedges, vector<Face *>* faces)
{
    Halfedge * h;
    int old_size = halfedges->size();

    //pour chaque arete
    for (int i = 0; i < old_size; ++i)
    {
        h = halfedges->at(i);

        //si l'arete n'a pas deja été traitée par sa paire
        if (h->done == false)
        {
            if(h->he_e)
            {
                Halfedge * h_e = h->he_e;

                //on subdivise les deux
                Halfedge * h2 = h->subdivise();
                Halfedge * h_e2 = h_e->subdivise();
                halfedges->push_back(h2);
                halfedges->push_back(h_e2);

                //et on supprime l'un des sommets
                delete h_e->v;
                h_e->v = h->v;
                vertex->push_back(h->v);

                vector<Vertex*> neighbour1 = h_e2->v->getNeighbours();
                vector<Vertex*> neighbour2 = h2->v->getNeighbours();

                // The edge connects two vertices of valence 6
                if((neighbour1.size() == 6) && (neighbour2.size() == 6))
                {
                    // Calcul des coordonnées de h->v
                    Vertex* h_11 = h->getPrevious()->he_e->he_n->v;
                    Vertex* h_12 = h2->he_n->v;
                    Vertex* h_13 = h2->he_n->he_e->he_n->v;

                    Vertex* h_21 = h->getOrigin();
                    Vertex* h_22 = h2->v;

                    Vertex* h_31 = h_e2->he_n->he_e->he_n->v;
                    Vertex* h_32 = h_e2->he_n->v;
                    Vertex* h_33 = h_e->getPrevious()->he_e->he_n->v;

                    h->v->v.x = (-1/16)*h_11->v.x + (1/8)*h_12->v.x + (-1/16)*h_13->v.x + (1/2)*h_21->v.x + (1/2)*h_22->v.x + (-1/16)*h_31->v.x + (1/8)*h_32->v.x + (-1/16)*h_33->v.x;
                    h->v->v.y = (-1/16)*h_11->v.y + (1/8)*h_12->v.y + (-1/16)*h_13->v.y + (1/2)*h_21->v.y + (1/2)*h_22->v.y + (-1/16)*h_31->v.y + (1/8)*h_32->v.y + (-1/16)*h_33->v.y;
                    h->v->v.z = (-1/16)*h_11->v.z + (1/8)*h_12->v.z + (-1/16)*h_13->v.z + (1/2)*h_21->v.z + (1/2)*h_22->v.z + (-1/16)*h_31->v.z + (1/8)*h_32->v.z + (-1/16)*h_33->v.z;
                }
                // The edge connects a K-vertex (extraordinary vertice, K != 6) and a 6-vertex
                else if (((neighbour1.size() == 6) && (neighbour2.size() != 6)) || ((neighbour1.size() != 6) && (neighbour2.size() == 6)))
                {
                    Vertex* v_irregular;
                    Vertex* v_regular;
                    vector<Vertex*> neighbour_irregular;
                    vector<Vertex*> neighbour_regular;

                    if(neighbour1.size() != 6)
                    {
                        v_irregular = h_e2->v;
                        v_regular = h2->v;
                        neighbour_irregular = neighbour1;
                        neighbour_regular = neighbour2;
                    }
                    else
                    {
                        v_irregular = h2->v;
                        v_regular = h_e2->v;
                        neighbour_irregular = neighbour2;
                        neighbour_regular = neighbour1;
                    }




                    int K = neighbour_irregular.size();

                    if(K >= 5)
                    {
                        h->v->v.x = h->v->v.y = h->v->v.z = 0;

                        double weight = 0;
                        for(int i = 0; i < K; ++i)
                        {
                            weight = (1/4 + cos((2*PI*i)/K) + (1/2) * cos((4*PI*i)/K)) / K;

                            h->v->v.x += weight * neighbour_irregular.at(i)->v.x;
                            h->v->v.y += weight * neighbour_irregular.at(i)->v.y;
                            h->v->v.z += weight * neighbour_irregular.at(i)->v.z;
                        }

                        // Vertex* h_1 = h2->he_n->he_e->he_n->v;
                        // Vertex* h_3 = h_e->getPrevious()->he_e->he_n->v;


                    }
                    else if(K = 4)
                    {
                        Vertex* h_11 = h->getPrevious()->he_e->v;
                        //Vertex* h_12 = h2->he_n->he_e->he_n->v;

                        Vertex* h_21 = h->getPrevious()->he_e->he_n->v;
                        Vertex* h_22 = h2->v;

                        Vertex* h_31 = h_e2->he_n->v;
                        //Vertex* h_32 = h_e->getPrevious()->he_e->he_n->v;

                        h->v->v.x = /*(3/8)*h_12->v.x + */(-1/8)*h_21->v.x + (3/8)*h_22->v.x/* + (3/8)*h_32->v.x*/;
                        h->v->v.y = /*(3/8)*h_12->v.y + */(-1/8)*h_21->v.y + (3/8)*h_22->v.y/* + (3/8)*h_32->v.y*/;
                        h->v->v.z = /*(3/8)*h_12->v.z + */(-1/8)*h_21->v.z + (3/8)*h_22->v.z/* + (3/8)*h_32->v.z*/;
                    }
                    else if(K = 3)
                    {
                        Vertex* h_11 = h->getPrevious()->he_e->v;
                        //Vertex* h_12 = h2->he_n->he_e->he_n->v;

                        Vertex* h_21 = h2->v;

                        Vertex* h_31 = h_e2->he_n->v;
                        //Vertex* h_32 = h_e->getPrevious()->he_e->he_n->v;

                        h->v->v.x = (-1/12)*h_11->v.x +/* (9/24)*h_12->v.x +*/ (5/12)*h_21->v.x + (-1/12)*h_31->v.x/* + (9/24)*h_32->v.x*/;
                        h->v->v.y = (-1/12)*h_11->v.y +/* (9/24)*h_12->v.y +*/ (5/12)*h_21->v.y + (-1/12)*h_31->v.y/* + (9/24)*h_32->v.y*/;
                        h->v->v.z = (-1/12)*h_11->v.z +/* (9/24)*h_12->v.z +*/ (5/12)*h_21->v.z + (-1/12)*h_31->v.z/* + (9/24)*h_32->v.z*/;
                    }

                }
                // The edge connects two extraordinary vertices
                else if((neighbour1.size() != 6) && (neighbour2.size() != 6))
                {
                    int K1 = neighbour1.size();
                    int K2 = neighbour2.size();

                    if(K1 >= 5)
                    {
                        h->v->v.x = h->v->v.y = h->v->v.z = 0;

                        double weight = 0;
                        for(int i = 0; i < K1; ++i)
                        {
                            weight = (1/4 + cos((2*PI*i)/K1) + (1/2) * cos((4*PI*i)/K1)) / K1;

                            h->v->v.x += weight * neighbour1.at(i)->v.x;
                            h->v->v.y += weight * neighbour1.at(i)->v.y;
                            h->v->v.z += weight * neighbour1.at(i)->v.z;
                        }
                    }
                    else if(K1 = 4)
                    {
                        Vertex* h_11 = h->getPrevious()->he_e->v;

                        Vertex* h_21 = h->getPrevious()->he_e->he_n->v;
                        Vertex* h_22 = h2->v;

                        Vertex* h_31 = h_e2->he_n->v;

                        h->v->v.x = (-1/8)*h_21->v.x + (3/8)*h_22->v.x;
                        h->v->v.y = (-1/8)*h_21->v.y + (3/8)*h_22->v.y;
                        h->v->v.z = (-1/8)*h_21->v.z + (3/8)*h_22->v.z;
                    }
                    else if(K1 = 3)
                    {
                        Vertex* h_11 = h->getPrevious()->he_e->v;
                        Vertex* h_21 = h2->v;
                        Vertex* h_31 = h_e2->he_n->v;

                        h->v->v.x = (-1/12)*h_11->v.x + (5/12)*h_21->v.x + (-1/12)*h_31->v.x;
                        h->v->v.y = (-1/12)*h_11->v.y + (5/12)*h_21->v.y + (-1/12)*h_31->v.y;
                        h->v->v.z = (-1/12)*h_11->v.z + (5/12)*h_21->v.z + (-1/12)*h_31->v.z;
                    }



                    if(K2 >= 5)
                    {
                        double weight = 0;
                        for(int i = 0; i < K1; ++i)
                        {
                            weight = (1/4 + cos((2*PI*i)/K2) + (1/2) * cos((4*PI*i)/K2)) / K2;

                            h->v->v.x += weight * neighbour2.at(i)->v.x;
                            h->v->v.y += weight * neighbour2.at(i)->v.y;
                            h->v->v.z += weight * neighbour2.at(i)->v.z;
                        }
                    }
                    else if(K2 = 4)
                    {
                        Vertex* h_11 = h2->he_n->v;

                        Vertex* h_21 = h2->he_n->he_e->he_n->v;
                        Vertex* h_22 = h->getOrigin();

                        Vertex* h_31 = h_e2->he_n->v;

                        h->v->v.x = (-1/8)*h_21->v.x + (3/8)*h_22->v.x;
                        h->v->v.y = (-1/8)*h_21->v.y + (3/8)*h_22->v.y;
                        h->v->v.z = (-1/8)*h_21->v.z + (3/8)*h_22->v.z;
                    }
                    else if(K1 = 3)
                    {
                        Vertex* h_11 = h2->he_n->v;
                        Vertex* h_21 = h->getOrigin();
                        Vertex* h_31 = h_e->getPrevious()->getOrigin();

                        h->v->v.x = (-1/12)*h_11->v.x + (5/12)*h_21->v.x + (-1/12)*h_31->v.x;
                        h->v->v.y = (-1/12)*h_11->v.y + (5/12)*h_21->v.y + (-1/12)*h_31->v.y;
                        h->v->v.z = (-1/12)*h_11->v.z + (5/12)*h_21->v.z + (-1/12)*h_31->v.z;
                    }
                }

                h->done = true;
                h2->done = true;
                h_e->done = true;
                h_e2->done = true;
            }
            // Boundary edges
            else
            {
                //on subdivise les deux
                Halfedge * h2 = h->subdivise();
                halfedges->push_back(h2);

                //et on supprime l'un des sommets
                vertex->push_back(h->v);

                vector<Vertex*> neighbour1 = h->getOrigin()->getNeighbours();
                vector<Vertex*> neighbour2 = h2->v->getNeighbours();

                Halfedge* temp_n1 = h;
                Halfedge* temp_n2 = h;
                for(int i = 0; i < neighbour1.size()-2; ++i)
                {
                    temp_n1 = temp_n1->getPrevious()->he_e;
                    temp_n2 = temp_n2->he_n->he_e;
                }

                Vertex* h_1 = temp_n1->he_n->v;
                Vertex* h_2 = h->getOrigin();
                Vertex* h_3 = h2->v;
                Vertex* h_4 = temp_n2->he_n->v;

                h->v->v.x = (-1/16)*h_1->v.x + (9/16)*h_2->v.x + (9/16)*h_3->v.x + (-1/16)*h_4->v.x;
                h->v->v.y = (-1/16)*h_1->v.y + (9/16)*h_2->v.y + (9/16)*h_3->v.y + (-1/16)*h_4->v.y;
                h->v->v.z = (-1/16)*h_1->v.z + (9/16)*h_2->v.z + (9/16)*h_3->v.z + (-1/16)*h_4->v.z;

                h->done = true;
                h2->done = true;
            }
        }
    }

	int size = halfedges->size();
	for (int i = 0; i < size; ++i)
	{
		h = halfedges->at(i);
		h->done = false;
	}

	// Créer les faces
}

int main( int argc, char ** argv )
{
    int n=4;
    int nb_pas = 3;

    Vertex ** t_Maillage = (Vertex **) malloc(sizeof(Vertex *)*nb_pas*nb_pas);
    Vertex ** t_Vertex = (Vertex **) malloc(sizeof(Vertex *)*n*n*n);
    vector<Halfedge *> v_Halfedge = vector<Halfedge *>();
    vector<Face *> v_Face = vector<Face *>();

    t_Vertex[0*n+0] = new Vertex(gk::Point(2.0,0.0,0.0));
    t_Vertex[0*n+1] = new Vertex(gk::Point(6.0,2.0,0.0));
    t_Vertex[0*n+2] = new Vertex(gk::Point(10.0,4.0,0.0));
    t_Vertex[0*n+3] = new Vertex(gk::Point(14.0,0.0,0.0));

    t_Vertex[1*n+0] = new Vertex(gk::Point(1.0,1.0,4.0));
    t_Vertex[1*n+1] = new Vertex(gk::Point(5.0,4.0,4.0));
    t_Vertex[1*n+2] = new Vertex(gk::Point(10.0,5.0,4.0));
    t_Vertex[1*n+3] = new Vertex(gk::Point(13.0,0.0,4.0));

    t_Vertex[2*n+0] = new Vertex(gk::Point(0.0,2.0,8.0));
    t_Vertex[2*n+1] = new Vertex(gk::Point(4.0,3.0,8.0));
    t_Vertex[2*n+2] = new Vertex(gk::Point(10.0,3.0,8.0));
    t_Vertex[2*n+3] = new Vertex(gk::Point(12.0,0.0,8.0));

    t_Vertex[3*n+0] = new Vertex(gk::Point(2.0,0.0,12.0));
    t_Vertex[3*n+1] = new Vertex(gk::Point(6.0,2.0,12.0));
    t_Vertex[3*n+2] = new Vertex(gk::Point(10.0,4.0,12.0));
    t_Vertex[3*n+3] = new Vertex(gk::Point(14.0,0.0,12.0));

    //calcul de la surface
    DeCasteljau( t_Maillage, nb_pas, t_Vertex, n );
    cout << "DeCasteljau -> Ok" << endl;

    //transformation du maillage en halfedge
    Halfedge::maillageToHalfedge( t_Maillage, nb_pas, &v_Halfedge, &v_Face );
    cout << "maillageToHalfedge -> Ok" << endl;

    //calcul des normales
    Halfedge::computeNormals( t_Maillage, nb_pas, &v_Face );
    cout << "computeNormals -> Ok" << endl;

    //export en obj
    Halfedge::exportToObj( "export.obj", t_Maillage, nb_pas, &v_Face );
    cout << "exportToObj export -> Ok" << endl;

    vector<Vertex *> v_V1 = vector<Vertex *>();
    vector<Halfedge *> v_H1 = vector<Halfedge *>();
    vector<Face *> v_F1 = vector<Face *>();
    Halfedge::importFromObj( "export.obj", &v_V1, &v_H1, &v_F1 );
    cout << "importFromObj export -> Ok" << endl;

    Loop( &v_V1, &v_H1, &v_F1 );
    cout << "Loop -> Ok" << endl;

    Halfedge::exportToObj( "import_export.obj", &v_V1, &v_F1 );
    cout << "exportToObj import_export -> Ok" << endl;

    free(t_Maillage);
    free(t_Vertex);

    return EXIT_SUCCESS;
}

