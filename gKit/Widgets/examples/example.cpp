
#include <stdlib.h> // exit, needed by GL/glut.h
#include <GL/glew.h>

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h> // glutBitmapWidth, glutBitmapCharacter
#endif

#include <nvwidgets/nvGlutWidgets.h>

#include <stdio.h>

#include <SDL/SDL.h>

enum UIOption {
    OPTION_DIFF,
    OPTION_DXT5_YCOCG,
    OPTION_COMPRESS,
    OPTION_ANIMATE,
    OPTION_THUMBNAIL,
    OPTION_COUNT
};

static bool options[OPTION_COUNT];

static nv::GlutUIContext ui;

static int win_w = 512, win_h = 512;

static float errorScale = 4.0f;
static float compressionRate = 0.0f;

static GLuint texture = 0;

static void doUI();

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    doUI();

    glutSwapBuffers();
    glutPostRedisplay();
}

static void idle() {
    //~ glutPostRedisplay();
}

static void key(unsigned char k, int x, int y) {
    ui.keyboard(k, x, y);

    switch (k) {
    case 27:
    case 'q':
        exit(0);
        break;
    }
}

static void special(int key, int x, int y) {
    ui.specialKeyboard(key, x, y);
}


void resize(int w, int h) {
    ui.reshape(w, h);

    glViewport(0, 0, w, h);

    win_w = w;
    win_h = h;
}

static void mouse(int button, int state, int x, int y) {
    ui.mouse(button, state, x, y);
}

static void motion(int x, int y) {
    ui.mouseMotion(x, y);
}

static void doUI() {
    static nv::Rect none;

    ui.begin();

    ui.beginGroup(nv::GroupFlags_GrowDownFromLeft);

    ui.doCheckButton(none, "Enable compression", &options[OPTION_COMPRESS]);

    if (options[OPTION_COMPRESS]) {
        ui.beginGroup(nv::GroupFlags_GrowLeftFromTop | nv::GroupFlags_LayoutNoMargin);

        ui.doCheckButton(none, "Show difference", &options[OPTION_DIFF]);

        if (options[OPTION_DIFF]) {
            ui.doHorizontalSlider(none, 1.0f, 16.0f, &errorScale);
        }

        ui.endGroup();

        ui.beginGroup( nv::GroupFlags_GrowLeftFromTop);

        ui.doLabel( none, "Format");

        const char * formatLabel[] = { "YCoCg-DXT5", "DXT1" };
        int formatIdx = options[OPTION_DXT5_YCOCG] ? 0 : 1;
        ui.doComboBox(none, 2, formatLabel, &formatIdx);
        options[OPTION_DXT5_YCOCG] = (formatIdx == 0);

        ui.endGroup();
    }

    ui.doCheckButton(none, "Display dummy texture", &options[OPTION_THUMBNAIL]);

    if (options[OPTION_THUMBNAIL]) {
        nv::Rect textureRect(0, 0, 100, 100);

        ui.doTextureView(textureRect, &texture, textureRect);
    }

    ui.endGroup();

    if (options[OPTION_COMPRESS]) {
        ui.beginGroup(nv::GroupFlags_GrowDownFromRight);

        if (ui.doButton(none, "Benchmark")) {
            // doBenchmark = true;
        }

        if (compressionRate != 0.0f) {
            char text[256];
            sprintf(text, "%.2f Mpixels/sec", 100.0f);
            ui.doLabel(none, text);
        }

        ui.endGroup();
    }

    // Pass non-ui mouse events to the manipulator
    //updateManipulator(ui, manipulator);

    ui.end();
}


int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    
    glutInit(&argc, argv);
    glutInitWindowSize(win_w, win_h);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("UI example");

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialization failed\n");
        return 0;
    }

    if (!ui.init(win_w, win_h)) {
        printf("UI initialization failed\n");
        return 0;
    }
    glutReportErrors();

    unsigned int texture_data[] = {
        0xFFFF0000, 0xFF0000FF,
        0xFF00FF00, 0xFF00FF00,
    };
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutIdleFunc(idle);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutReshapeFunc(resize);

    glutMainLoop();

    return 0;
}
