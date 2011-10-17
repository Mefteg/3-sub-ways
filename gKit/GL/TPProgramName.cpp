
#include "GL/TPShaderProgram.h"
#include "GL/TPProgramName.h"


namespace gk {  

int setSamplerUniform( const ProgramSampler& sampler, const int unit )
{
    if(sampler.isValid())
    {
        ActiveShaderProgram.setShaderProgram(sampler.program());
        glUniform1i(sampler.location(), unit);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const unsigned int x )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform1ui(uniform.location(), x);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform2ui(uniform.location(), x, y);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y, const unsigned int z )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform3ui(uniform.location(), x, y, z);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int w )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform4ui(uniform.location(), x, y, z, w);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const int x )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform1i(uniform.location(), x);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const int x, const int y )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform2i(uniform.location(), x, y);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const int x, const int y, const int z )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform3i(uniform.location(), x, y, z);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const int x, const int y, const int z, const int w )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform4i(uniform.location(), x, y, z, w);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const float x )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform1f(uniform.location(), x);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const float x, const float y )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform2f(uniform.location(), x, y);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const float x, const float y, const float z )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform3f(uniform.location(), x, y, z);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const float x, const float y, const float z, const float w )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniform4f(uniform.location(), x, y, z, w);
        return 0;
    }
    return -1;
}

int setUniform( const ProgramUniform& uniform, const float *m, const GLboolean transpose )
{
    if(uniform.isValid())
    {
        ActiveShaderProgram.setShaderProgram(uniform.program());
        glUniformMatrix4fv(uniform.location(), 1, transpose, m);
        return 0;
    }
    return -1;
}

}
