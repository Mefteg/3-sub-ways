
// a file selector using nvWidgets
// mailto:jean-claude.iehl@liris.cnrs.fr

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

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
        printf("\n -- failed: '%s'\n", SDL_GetError());
        return -1;
    }

    int keys_n;
    keys= SDL_GetKeyState(&keys_n);
    SDL_WM_SetCaption("SDLkit", "");

    atexit(SDL_Quit);
    sdlkit_resize(w, h);
    return 0;
}



class FileWidget
{
    nv::UIContext& m_ui;
    std::string m_pathname;
    
    char m_edit[256];
    int m_edit_length;
    
    std::vector<std::string> m_combo_dirs;
    std::vector<std::string> m_button_dirs;
    std::string m_combo_dir;
    int m_combo_select;
    
    std::vector<const char *> m_combo_tmps;

    unsigned char last_char( const char *string )
    {
        if(string[0] == 0)
            return 0;
        int i;
        for(i= 0; string[i] != 0; i++)
            {;}
        return string[i-1];
    }

    void get_combo_dirs( const std::string& basepath )
    {
        if(m_combo_dir == basepath)
        {
            if(m_combo_dirs.empty() == false)
                m_combo_select= (m_combo_select +1) % (int) m_combo_dirs.size();
            return;
        }
        
        printf("get_combo_dir( ): %s\n", basepath.c_str());
        m_combo_dir= basepath;
        m_combo_dirs.clear();
        m_combo_tmps.clear();
        m_combo_select= -1;

        DIR *dir= opendir(basepath.c_str());
        if(dir == NULL)
            return;
        
        struct dirent *entry= readdir(dir);
        while(entry != NULL)
        {
            struct stat entry_stat;
            std::string pathname= basepath + entry->d_name;
            
            if(stat(pathname.c_str(), &entry_stat) == 0
            && S_ISDIR(entry_stat.st_mode)
            && entry->d_name[0] != '.')
            {
                m_combo_dirs.push_back(std::string(entry->d_name) + '/');
                m_combo_tmps.push_back(m_combo_dirs.back().c_str());
                m_combo_select= 0;
            }
            
            entry= readdir(dir);
        }
        closedir(dir);
    }
    
    void get_button_dirs( const std::string& pathname )
    {
        m_button_dirs.clear();
        
        int begin= 0;
        for(int i= 0; pathname[i] != 0; i++)
        {
            if(pathname[i] == '/' || pathname[i] == '\\')
            {
                m_button_dirs.push_back(pathname.substr(begin, i - begin +1));
                begin= i+1;
            }
        }
    }
    
    void update_edit( )
    {
        m_edit[0]= 0;
        const int depth= (int) m_button_dirs.size();
        for(int i= 0; i < depth; i++)
        {
            printf("  button %s\n", m_button_dirs[i].c_str());
            strncat(m_edit, m_button_dirs[i].c_str(), sizeof(m_edit));
        }
        
        m_edit_length= strlen(m_edit);
        printf("update_edit( ): %s\n", m_edit);
    }
    
public:
    FileWidget( nv::UIContext& ui, const std::string& pathname= "./" )
        :
        m_ui(ui),
        m_pathname(pathname),
        m_edit_length(0),
        m_combo_select(0)
    {
        strncpy(m_edit, pathname.c_str(), sizeof(m_edit));
    }
    
    bool doFile( const nv::Rect& rect, std::string& filename )
    {
        m_ui.beginGroup(nv::GroupFlags_GrowDownFromLeft);
        {
            if(m_ui.doLineEdit(rect, m_edit, sizeof(m_edit), &m_edit_length))
            {
                if(last_char(m_edit) == '/' && keys['\t'])
                {
                    // mettre a jour la liste des repertoires disponibles
                    get_button_dirs(filename + m_edit);
                    get_combo_dirs(filename + m_edit);
                }
                else if(m_combo_select != -1 && last_char(m_edit) == '/' && keys['\r'])
                {
                    strncat(m_edit, m_combo_tmps[m_combo_select], sizeof(m_edit));
                    m_edit_length= strlen(m_edit);
                    m_combo_select= -1;
                    get_combo_dirs(filename + m_edit);
                    get_button_dirs(filename + m_edit);
                }
            }
            
            m_ui.beginGroup(nv::GroupFlags_GrowRightFromTop);
            {
                // affiche la path bar
                const int button_n= (int) m_button_dirs.size();
                for(int i= 0; i < button_n; i++)
                    if(m_ui.doButton(nv::Rect(), m_button_dirs[i].c_str()))
                    {
                        // se repositionner sur le repertoire associe au bouton
                        m_button_dirs.resize(i);
                        m_combo_select= -1;
                        update_edit();
                        // mettre a jour la liste des repertoires disponibles
                        get_button_dirs(filename + m_edit);
                        get_combo_dirs(filename + m_edit);
                        break;
                    }
                
                // affiche la liste de navigation, si necessaire
                if(m_combo_tmps.size() > 0 
                && m_ui.doComboBox(nv::Rect(), (int) m_combo_tmps.size(), (const char **) &m_combo_tmps.front(), &m_combo_select))
                {
                    strncat(m_edit, m_combo_tmps[m_combo_select], sizeof(m_edit));
                    m_edit_length= strlen(m_edit);
                    get_combo_dirs(filename + m_edit);
                    get_button_dirs(filename + m_edit);
                }
            }
            m_ui.endGroup();
        }
        m_ui.endGroup();
        
        return false;
    }
    
    ~FileWidget( ) {}
};


int main( )
{
    sdlkit_init(512, 512);
    
    nv::SdlUIContext ui;
    if (!ui.init(512, 512)) {
        printf("\n -- failed: 'SdlUIContext initialization error'.\n");
        return 1;
    }    
    
    FileWidget selector(ui, "./");
    
    while(!stop)
    {
        // integrate ui event loop in main loop
        {
            SDL_Event event;
            
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:
                        ui.processKeyboardEvent(event.key);
                        break;
                    
                    case SDL_MOUSEMOTION:
                        ui.processMouseMotionEvent(event.motion);
                        break;
                    
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:
                        ui.processMouseButtonEvent(event.button);
                        break;
                    
                    case SDL_VIDEORESIZE:
                        sdlkit_resize(event.resize.w, event.resize.h);
                        ui.reshape(event.resize.w, event.resize.h);
                        break;
                    
                    case SDL_QUIT:
                        stop= 1;
                        break;
                }
            }
        }
        
        // application
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
            std::string filename;
            if(selector.doFile(nv::Rect(), filename))
                printf("file select %s\n", filename.c_str());
        ui.end();
        
        SDL_GL_SwapBuffers();
        
        SDL_Delay(20);
    }
    
    return 0;
}
