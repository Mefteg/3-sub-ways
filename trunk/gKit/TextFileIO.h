
#ifndef _IOTEXTFILE_H
#define _IOTEXTFILE_H

#include "IOManager.h"
#include "TextFile.h"

namespace gk {

//! manager pour importer des ficheirs textes, sources, shaders, etc.
class TextFileIO : public IOManager<TextFile>
{
    // non copyable
    TextFileIO( const TextFileIO& );
    TextFileIO& operator=( const TextFileIO& );
    
    // private default constructor, singleton
    TextFileIO( )
        :
        IOManager<TextFile>()
    {}
    
    ~TextFileIO( ) {}
    
public:
    //! importe l'objet 'name' du fichier 'filename'.
    static
    TextFile *read( const std::string& filename, const std::string& name= "" ) 
    {
        // importer le fichier, si necessaire
        if(filename.empty() == true)
            return NULL;
        
        TextFile *file= manager().find(filename, name);
        if(file != NULL)
            return file;
        
        // importer le fichier
        file= new TextFile(filename);
        if(file->read(filename) < 0)
        {
            delete file;
            return NULL;
        }
        
        // reference le mesh avec le manager
        return manager().insert(file, filename, name);
    }

    static
    TextFileIO& manager( )  // singleton
    {
        static TextFileIO manager;
        return manager;
    }
};

} // namespace

#endif
