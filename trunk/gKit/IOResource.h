
#ifndef _IORESOURCE_H
#define _IORESOURCE_H

namespace gk {

//! classe de base des resources importees depuis un fichier, referencees par l'application, proprietes du gk::IOManager.
class IOResource
{
    // non copyable
    IOResource( const IOResource& );
    IOResource& operator=( const IOResource& );
    
public:
    IOResource( ) {}
    virtual ~IOResource( ) {}
};

} // namespace

#endif
