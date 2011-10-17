
#include <cstdio>
#include <cassert>

#include "GL/TPTexture.h"
#include "GL/TPSampler.h"
#include "GL/TPTextureUnits.h"


namespace gk {

TextureUnitState TextureUnits[16];
TextureUnitState ActiveTextureUnits[16];
TextureUnitState DefaultUnit;
int TextureUnitCount= 0;
int ActiveUnit= 0;

int TextureUnitState::init( )
{
    for(int i= 0; i < 16; i++)
    {
        ActiveTextureUnits[i]= TextureUnitState(i);
        TextureUnits[i]= ActiveTextureUnits[i];
    }
    
    return 0;
}

TextureUnitState& setTexture( const ProgramSampler& unit, GLTexture *texture )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].setTexture(unit, texture);
    return TextureUnits[unit.index()];
}

TextureUnitState& setSampler( const ProgramSampler& unit, GLSampler *sampler )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].setSampler(unit, sampler);
    return TextureUnits[unit.index()];
}

TextureUnitState& setTextureUnit( const ProgramSampler& unit, GLTexture *texture, GLSampler *sampler )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].setTextureUnit(unit, texture, sampler);
    return TextureUnits[unit.index()];
}

TextureUnitState& resetTexture( const ProgramSampler& unit )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].resetTexture();
    return TextureUnits[unit.index()];
}

TextureUnitState& resetSampler( const ProgramSampler& unit )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].resetSampler();
    return TextureUnits[unit.index()];
}

TextureUnitState& resetTextureUnit( const ProgramSampler& unit )
{
    if(unit.index() < 0)
        return DefaultUnit;
    
    TextureUnits[unit.index()]= ActiveTextureUnits[unit.index()].reset();
    return TextureUnits[unit.index()];
}


TextureUnitState& TextureUnitState::setTextureUnit( const ProgramSampler& unit, GLTexture *texture, GLSampler *sampler )
{
    assert(m_index != -1);
    assert(m_index == unit.index());
    assert(texture != NULL);
    assert(sampler != NULL);
    
    // active l'unite de texture
    if(ActiveUnit != unit.index())
        glActiveTexture(GL_TEXTURE0 + unit.index());
    
    // active la texture
    if(m_texture != NULL && m_texture->target() != texture->target())
        glBindTexture(m_texture->target(), 0);
    
    // active le type de la "nouvelle" texture
    if(m_texture != texture)
        glBindTexture(texture->target(), texture->name());

    // active le sampler
    if(m_sampler != sampler)
        glBindSampler(unit.index(), sampler->name());
    
    // parametre le shader
    setSamplerUniform(unit, unit.index());

    // conserve l'etat
    ActiveUnit= unit.index();
    m_texture= texture;
    m_sampler= sampler;
    return *this;
}

TextureUnitState& TextureUnitState::reset( )
{
    assert(m_index != -1);
    if(m_texture == NULL)
        return *this;
    
    if(ActiveUnit != m_index)
        glActiveTexture(GL_TEXTURE0 + m_index);
    
    if(m_texture != NULL)
        glBindTexture(m_texture->target(), 0);
    
    glBindSampler(m_index, 0);
    
    m_texture= NULL;
    m_sampler= NULL;
    ActiveUnit= m_index;
    return *this;
}

TextureUnitState& TextureUnitState::setTexture( const ProgramSampler& unit, GLTexture *texture )
{
    assert(m_index != -1);
    assert(texture != NULL);
    
    if(ActiveUnit != unit.index())
        glActiveTexture(GL_TEXTURE0 + unit.index());
    
    if(m_texture != NULL && m_texture->target() != texture->target())
        glBindTexture(m_texture->target(), 0);
    
    if(m_texture != texture)
        glBindTexture(texture->target(), texture->name());

    m_texture= texture;
    m_index= unit.index();
    ActiveUnit= unit.index();
    return *this;
}

TextureUnitState& TextureUnitState::resetTexture( )
{
    assert(m_index != -1);
    if(ActiveUnit != m_index)
        glActiveTexture(GL_TEXTURE0 + m_index);

    glBindTexture(m_texture->target(), 0);
    
    m_texture= NULL;
    ActiveUnit= m_index;
    return *this;
}

TextureUnitState& TextureUnitState::setSampler( const ProgramSampler& unit, GLSampler *sampler )
{
    assert(m_index != -1);
    assert(m_index == unit.index());
    assert(sampler != NULL);
    
    if(m_sampler != sampler)
        glBindSampler(unit.index(), sampler->name());

    m_sampler= sampler;
    return *this;
}

TextureUnitState& TextureUnitState::resetSampler( )
{
    assert(m_index != -1);
    glBindSampler(m_index, 0);
    m_sampler= NULL;
    return *this;
}

}
