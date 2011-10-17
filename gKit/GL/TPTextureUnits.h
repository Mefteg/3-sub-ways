
#ifndef _TP_TEXTUREUNIT_H
#define _TP_TEXTUREUNIT_H

#include "GL/GLPlatform.h"
#include "GL/TPProgramName.h"


namespace gk {

class GLTexture;
class GLSampler;
    
class TextureUnitState
{
    GLTexture *m_texture;
    GLSampler *m_sampler;
    int m_index;
    
    TextureUnitState( const int index )
        :
        m_texture(NULL),
        m_sampler(NULL),
        m_index(index)
    {}
    
public:
    TextureUnitState( )
        :
        m_texture(NULL),
        m_sampler(NULL),
        m_index(-1)
    {}
    
    TextureUnitState& setIndex( const int index )
    {
        m_index= index;
        return *this;
    }
    
    int getIndex( )
    {
        return m_index;
    }
    
    TextureUnitState& setTextureUnit( const ProgramSampler& unit, GLTexture *texture, GLSampler *sampler );
    TextureUnitState& reset( );
    
    TextureUnitState& setTexture( const ProgramSampler& unit, GLTexture *texture );  
    TextureUnitState& resetTexture( );
    GLTexture *getTexture( )
    {
        return m_texture;
    }
    
    TextureUnitState& setSampler( const ProgramSampler& unit, GLSampler *sampler );
    TextureUnitState& resetSampler( );
    GLSampler *getSampler( )
    {
        return m_sampler;
    }
    
    //! initilisation du suivi des etats des unites de textures. appel necessaire avant toute utilisation.
    static int init( );
};

enum
{
    UNIT0= 0,
    UNIT1= 1,
    UNIT2= 2,
    UNIT3= 3,
    UNIT4= 4,
    UNIT5= 5,
    UNIT6= 6,
    UNIT7= 7
};

extern TextureUnitState TextureUnits[16];
extern TextureUnitState ActiveTextureUnits[16];
extern int ActiveUnit;
extern int TextureUnitCount;

TextureUnitState& setTexture( const ProgramSampler& unit, GLTexture *texture );
TextureUnitState& setSampler( const ProgramSampler& unit, GLSampler *sampler );
TextureUnitState& setTextureUnit( const ProgramSampler& unit, GLTexture *texture, GLSampler *sampler );
TextureUnitState& resetTexture( const ProgramSampler& unit );
TextureUnitState& resetSampler( const ProgramSampler& unit );
TextureUnitState& resetTextureUnit( const ProgramSampler& unit );

}

#endif
