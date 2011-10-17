
#ifndef _IOFILESYSTEM_H
#define _IOFILESYSTEM_H

#ifdef APPLE_OSX
    #include <sys/stat.h>
    
#elif defined WIN32
    #include <sys/types.h>
    #include <sys/stat.h>
    
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include <cstdio>
#include <string>


namespace gk {

struct IOFileSystem
{
    //! renvoie le chemin d'acces a un fichier.

    /*! le chemin est toujours termine par /
     pathname("path/to/file") == "path/to/"
     pathname("file") == "./"
     */
    static
    std::string pathname( const std::string& filename )
    {
        size_t slash = filename.find_last_of( '/' );
        size_t bslash = filename.find_last_of( '\\' );
        
        if ( slash == std::string::npos && bslash != std::string::npos )
            slash = bslash;
        else if ( slash != std::string::npos && bslash != std::string::npos && bslash > slash )
            slash = bslash;
            
        if ( slash != std::string::npos )
            return filename.substr( 0, slash +1 ); // inclus le slash
        else
            return "./";
    }
    
    //! renvoie le nom du fichier sans son extension (chemin inclus).
    static
    std::string basename( const std::string& filename )
    {
        size_t ext= filename.find_last_of('.');
        if(ext== std::string::npos)
            return filename;    // renvoie le nom tel quel
        else
            return filename.substr(0, ext);  // n'inclus pas le point
    }
    
    //! verifie que le fichier est bien du type represente par 'suffix'.
    static
    bool isType( const std::string& filename, const std::string& suffix )
    {
        size_t ext= filename.find_last_of('.');
        if(ext != std::string::npos)
            return (filename.substr(ext, std::string::npos).rfind(suffix) != std::string::npos);
        else
            return (filename.rfind(suffix) != std::string::npos);
    }
    
    //! change l'extension du fichier.
    static
    std::string changeType( const std::string& filename, const std::string& suffix )
    {
        size_t ext= filename.find_last_of('.');
        if(ext == std::string::npos)
            return filename + suffix;
        else
            return filename.substr(0, ext) + suffix;
    }
    
    //! verifie l'existance d'un fichier.
    static
    bool exists( const std::string& filename )
    {
    #ifndef WIN32
        struct stat info;
        if(stat(filename.c_str(), &info) < 0)
            return -1;
        
        // verifie aussi que c'est bien un fichier standard
        return (S_ISREG(info.st_mode));
        
    #else
        struct _stat64 info;
        if(_stat64(filename.c_str(), &info) < 0)
            return -1;
        
        // verifie aussi que c'est bien un fichier standard
        return ((info.st_mode & _S_IFREG) == _S_IFREG);
    #endif
    }
    
    //! renvoie les informations sur un fichier.
    static
    int infos( const std::string& filename, size_t *size, size_t *time )
    {
    #ifndef WIN32
        struct stat info;
        if(stat(filename.c_str(), &info) < 0)
            return -1;
        
        if(size != NULL)
            *size= info.st_size;
        if(time != NULL)
            *time= info.st_mtime;
        return 0;
        
    #else
        struct _stat64 info;
        if(_stat64(filename.c_str(), &info) < 0)
            return -1;
        
        if(size != NULL)
            *size= info.st_size;
        if(time != NULL)
            *time= info.st_mtime;
        return 0;
    #endif
    }
    
    //! compare 2 dates de fichiers.
    //! renvoie -1 si a est plus vieux que b et 1 dans le cas contraire.
    static
    int compare( const size_t a, const size_t b )
    {
        if(a < b)
            return -1;
        else if(a > b)
            return 1;
        else
            return 0;
    }
    
    //! verifie que b est plus recent que a, si b existe.
    //! renvoie 0 si b n'existe pas et -1 en cas d'erreur (a n'existe pas).
    static
    int uptodate( const std::string& filea, const std::string& fileb )
    {
        size_t timea;
        if(infos(filea, NULL, &timea) < 0)
            return -1;
        
        size_t timeb;
        if(infos(fileb, NULL, &timeb) < 0)
            return 0;   // le fichier n'existe pas, il n'est pas a jour ...
        
        return (compare(timea, timeb) <= 0);
        // b a ete cree apres a, donc timeB > timeA
    }
};

}

#endif
