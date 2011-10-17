
#include "EffectIO.h"
#include "EffectShaderManager.h"


namespace gk {

EffectShaderManager::EffectShaderManager( const std::string& effect )
    :
    m_effect(NULL)
{
    m_effect= EffectIO::read(effect);
}

EffectShaderManager::EffectShaderManager( Effect *effect )
    :
    m_effect(effect)
{}

EffectShaderManager::~EffectShaderManager( ) {}

GL2ShaderProgram *EffectShaderManager::createShaderProgram2( const std::string& program_key )
{
    if(m_effect == NULL)
        return NULL;
    
    TextFile *program= m_effect->find(program_key);
    if(program == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram2('%s'): key not found.\n",
            program_key.c_str());
    #endif
        return NULL;
    }

    return createShaderProgram2( 
        program->find("vertex"), 
        program->find("fragment"));
}

GL2ShaderProgram *EffectShaderManager::createShaderProgram2( 
    const TextValue& vertex_key, 
    const TextValue& fragment_key )
{
    if(m_effect == NULL)
        return NULL;
    
    TextFile *vertex= NULL;
    if(vertex_key != TextValue::notFound())
        vertex= m_effect->find(vertex_key.asName());
    if(vertex == NULL || vertex == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram2( ): vertex key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *fragment= NULL;
    if(fragment_key != TextValue::notFound())
        fragment= m_effect->find(fragment_key.asName());
    if(fragment == NULL || fragment == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram2( ): fragment key not found.\n");
    #endif
        return NULL;
    }

#ifdef VERBOSE
    printf("EffectShaderManager::createShaderProgram2( ): vertex key '%s', fragment key '%s'...\n", vertex_key.asName().c_str(), fragment_key.asName().c_str());
#endif
    return GLManager<GL2ShaderProgram>::manager().insert(
        new GL2ShaderProgram(vertex, fragment) );
}

#if defined GK_OPENGL3 || defined GK_OPENGL4    
GL3ShaderProgram *EffectShaderManager::createShaderProgram3( const std::string& program_key )
{
    if(m_effect == NULL)
        return NULL;
    TextFile *program= m_effect->find(program_key);
    if(program == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram3('%s'): key not found.\n",
            program_key.c_str());
    #endif
        return NULL;
    }
    
    return createShaderProgram3( 
        program->find("vertex"), 
        program->find("geometry"), 
        program->find("fragment") );
}

GL3ShaderProgram *EffectShaderManager::createShaderProgram3( 
    const TextValue& vertex_key,
    const TextValue& geometry_key,
    const TextValue& fragment_key )
{
    if(m_effect == NULL)
        return NULL;
    
    TextFile *vertex= NULL;
    if(vertex_key != TextValue::notFound())
        vertex= m_effect->find(vertex_key.asName());
    if(vertex == NULL || vertex == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram3( ): vertex key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *geometry= NULL;
    if(geometry_key != TextValue::notFound())
        geometry= m_effect->find(geometry_key.asName());
    if(geometry == NULL || geometry == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram3( ): geometry key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *fragment= NULL;
    if(fragment_key != TextValue::notFound())
        fragment= m_effect->find(fragment_key.asName());
    if(fragment == NULL || fragment == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram3( ): fragment key not found.\n");
    #endif
        return NULL;
    }
    
#ifdef VERBOSE
    printf("EffectShaderManager::createShaderProgram3( ): vertex key '%s', geometry key '%s', fragment key '%s'...\n", 
        vertex_key.asName().c_str(), geometry_key.asName().c_str(),  fragment_key.asName().c_str());
#endif
    return GLManager<GL3ShaderProgram>::manager().insert(
        new GL3ShaderProgram(vertex, geometry, fragment) );
}

#endif
#ifdef GK_OPENGL4    
GL4ShaderProgram *EffectShaderManager::createShaderProgram4( const std::string& program_key )
{
    if(m_effect == NULL)
        return NULL;
    TextFile *program= m_effect->find(program_key);
    if(program == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4('%s'): key not found.\n",
            program_key.c_str());
    #endif
        return NULL;
    }
    
    return createShaderProgram4( 
        program->find("vertex"), 
        program->find("control"), 
        program->find("evaluation"), 
        program->find("geometry"), 
        program->find("fragment") );
}

GL4ShaderProgram *EffectShaderManager::createShaderProgram4( 
    const TextValue& vertex_key,
    const TextValue& control_key,
    const TextValue& evaluation_key,
    const TextValue& geometry_key,
    const TextValue& fragment_key )
{
    if(m_effect == NULL)
        return NULL;
    
    TextFile *vertex= NULL;
    if(vertex_key != TextValue::notFound())
        vertex= m_effect->find(vertex_key.asName());
    if(vertex == NULL || vertex == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4( ): vertex key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *control= NULL;
    if(control_key != TextValue::notFound())
        control= m_effect->find(control_key.asName());
    if(control == NULL || control == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4( ): tesselation control key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *evaluation= NULL;
    if(evaluation_key != TextValue::notFound())
        evaluation= m_effect->find(evaluation_key.asName());
    if(evaluation == NULL || evaluation == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4( ): tesselation evaluation key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *geometry= NULL;
    if(geometry_key != TextValue::notFound())
        geometry= m_effect->find(geometry_key.asName());
    if(geometry == NULL || geometry == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4( ): geometry key not found.\n");
    #endif
        return NULL;
    }
    
    TextFile *fragment= NULL;
    if(fragment_key != TextValue::notFound())
        fragment= m_effect->find(fragment_key.asName());
    if(fragment == NULL || fragment == TextFile::notFound())
    {
    #ifdef VERBOSE
        printf("EffectShaderManager::createShaderProgram4( ): fragment key not found.\n");
    #endif
        return NULL;
    }
    
#ifdef VERBOSE
    printf("EffectShaderManager::createShaderProgram4( ): vertex key '%s', control key '%s', evaluation key '%s', geometry key '%s', fragment key '%s'...\n",
        vertex_key.asName().c_str(), 
        control_key.asName().c_str(), evaluation_key.asName().c_str(), 
        geometry_key.asName().c_str(), fragment_key.asName().c_str());
#endif
    return GLManager<GL4ShaderProgram>::manager().insert(
        new GL4ShaderProgram(vertex, control, evaluation, geometry, fragment) );
}
#endif

}
