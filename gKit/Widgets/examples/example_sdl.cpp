
// basic example using nvSdlWidgets
// mailto:jean-claude.iehl@liris.cnrs.fr

#include <SDL/SDL.h>
#include "nvSdlWidgets.h"


static volatile int stop= 0;
static unsigned char *keys= NULL;
static const SDL_VideoInfo *screen_info= NULL;
static SDL_Surface *screen= NULL;
static int width= 0, height= 0;

int sdlkit_resize(int w, int h)
{
    screen_info= SDL_GetVideoInfo();
    if(screen_info==NULL)
    {
        printf("\n -- failed: '%s'\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    width= w;
    height= h;
    screen= SDL_SetVideoMode(w, h, 
        screen_info->vfmt->BitsPerPixel, 
        SDL_RESIZABLE | SDL_OPENGL | SDL_HWSURFACE);
    
    if(screen==NULL)
    {
        printf("\n -- failed: '%s'\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    return 0;
}

void sdlkit_get_viewport(int viewport[4])
{
    viewport[0]= 0;
    viewport[1]= 0;
    viewport[2]= width;
    viewport[3]= height;
}

void sdlkit_events(void)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_VIDEORESIZE:
                sdlkit_resize(event.resize.w, event.resize.h);
                break;
    
            case SDL_QUIT:
                stop= 1;
                break;
        }
    }
}

int sdlkit_init(int w, int h)
{
    if(SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_EVERYTHING) < 0)
    { 
        printf("\n -- failed : '%s'\n", SDL_GetError());
        return -1;
    }

    int keys_n;
    keys= SDL_GetKeyState(&keys_n);
    SDL_WM_SetCaption("SDLkit", "");

    atexit(SDL_Quit);
    sdlkit_resize(w, h);
    return 0;
}

int main( )
{
    sdlkit_init(512, 512);
    
    nv::SdlUIContext ui;
    if (!ui.init(512, 512)) {
        printf("UI initialization failed\n");
        return 1;
    }    
    
    bool click;
    float value;
    int edit_length;
    char edit[256]= { 0 };
    
    while(!stop)
    {
        sdlkit_events();
        
        ui.processState();
        if(keys[SDLK_ESCAPE])
            break;
        
        int viewport[4];
        sdlkit_get_viewport(viewport);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, viewport[2], 0, viewport[3], -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ui.begin();
            ui.beginGroup(nv::GroupFlags_GrowDownFromLeft);
                ui.doCheckButton(nv::Rect(), "button", &click);
                if(click)
                    printf("button !!\n");
                ui.doHorizontalSlider(nv::Rect(), 1.0f, 16.0f, &value);
                printf("slider %f\n", value);
                
                ui.doLineEdit(nv::Rect(), edit, sizeof(edit), &edit_length);
                printf("edit (%d): %s\n", edit_length, edit);
            ui.endGroup();
        ui.end();
        
        SDL_GL_SwapBuffers();
        
        SDL_Delay(20);
    }
    
    return 0;
}
