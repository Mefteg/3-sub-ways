
#include <cassert>

#include "GL/TPBuffer.h"
#include "GL/TPAttributes.h"


namespace gk {

BufferState Attributes[BUFFER_STATE_LAST];
BufferState ActiveAttributes[BUFFER_STATE_LAST];

BufferState DefaultAttribute;
int AttributeCount= 0;
bool AttributesDirty= false;
bool IndexDirty= false;
    
int BufferState::init( )
{
    int active= INDEX;
    ActiveAttributes[INDEX]= BufferState(GL_ELEMENT_ARRAY_BUFFER, active);
    Attributes[INDEX]= ActiveAttributes[INDEX];
    
    active= ATTRIBUTE;
    ActiveAttributes[ATTRIBUTE]= BufferState(GL_ARRAY_BUFFER, active);
    Attributes[ATTRIBUTE]= ActiveAttributes[ATTRIBUTE];
    for(int i= 0; i < 8; i++)
    {
        ActiveAttributes[ATTRIBUTE0 + i]= BufferState(GL_ARRAY_BUFFER, active, i);
        Attributes[ATTRIBUTE0 + i]= ActiveAttributes[ATTRIBUTE0 + i];
    }
    
    active= FEEDBACK;
    ActiveAttributes[FEEDBACK]= BufferState(GL_TRANSFORM_FEEDBACK_BUFFER, active);
    Attributes[FEEDBACK]= ActiveAttributes[FEEDBACK];
    for(int i= 0; i < 4; i++)
    {
        ActiveAttributes[FEEDBACK0 + i]= BufferState(GL_TRANSFORM_FEEDBACK_BUFFER, active, i);
        Attributes[FEEDBACK0 + i]= ActiveAttributes[FEEDBACK0 + i];
    }
    
    active= UNIFORM;
    ActiveAttributes[UNIFORM]= BufferState(GL_UNIFORM_BUFFER, active);
    Attributes[UNIFORM]= ActiveAttributes[UNIFORM];
    for(int i= 0; i < 8; i++)
    {
        ActiveAttributes[UNIFORM0 + i]= BufferState(GL_UNIFORM_BUFFER, active, i);
        Attributes[UNIFORM0 + i]= ActiveAttributes[UNIFORM0 + i];
    }
    
#ifdef GK_OPENGL4
    active= INDIRECT;
    ActiveAttributes[INDIRECT]= BufferState(GL_DRAW_INDIRECT_BUFFER, active);
    Attributes[INDIRECT]= ActiveAttributes[INDIRECT];
#endif
    
    return 0;
}

BufferState& setVertexBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout )
{
    if(attribute.location() < 0)
        return DefaultAttribute;
    
    Attributes[ATTRIBUTE0 + attribute.location()]= 
        ActiveAttributes[ATTRIBUTE0 + attribute.location()].setBufferState(attribute, buffer, layout);
    return Attributes[ATTRIBUTE0 + attribute.location()];
}

BufferState& setVertexBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride, const unsigned int offset, const unsigned int flags )
{
    return setVertexBuffer( attribute, buffer, BufferLayout(size, type, stride, offset, 0, flags) );
}

BufferState& resetVertexBuffer( const ProgramAttribute& attribute )
{
    if(attribute.location() < 0)
        return DefaultAttribute;
    
    Attributes[ATTRIBUTE0 + attribute.location()]= 
        ActiveAttributes[ATTRIBUTE0 + attribute.location()].resetVertexBuffer();
    return Attributes[ATTRIBUTE0 + attribute.location()];
}

BufferState& setInstanceBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout )
{
#ifdef VERBOSE
    if(layout.divisor == 0)
        printf("setInstanceBuffer( ): divisor= 0\n");
#endif
    return setVertexBuffer(attribute, buffer, layout);
}

BufferState& setInstanceBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride, const unsigned int offset, const unsigned int flags, const int divisor )
{
    return setInstanceBuffer( attribute, buffer, BufferLayout(size, type, stride, offset, divisor, flags) );
}

BufferState& resetInstanceBuffer( const ProgramAttribute& attribute )
{
    return resetVertexBuffer(attribute);
}


BufferState& setIndexBuffer( GLIndexBuffer *buffer, const BufferLayout& layout )
{
    Attributes[INDEX]= 
        ActiveAttributes[INDEX].setBufferState(buffer, layout);
    return Attributes[INDEX];
}

BufferState& setIndexBuffer( GLIndexBuffer *buffer, const GLenum type, const unsigned int offset )
{
    return setIndexBuffer(buffer, BufferLayout(1, type, 0, offset) );
}

BufferState& resetIndexBuffer( )
{
    Attributes[INDEX]= 
        ActiveAttributes[INDEX].resetIndexBuffer();
    return Attributes[INDEX];
}



BufferState& BufferState::setBufferState( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout )
{
    assert(m_index != -1);
    assert(buffer != NULL);
    assert(m_target == GL_ARRAY_BUFFER);
    assert(attribute.location() == m_index);
    
    if(m_buffer != buffer || m_layout != layout)
    {
        if(ActiveAttributes[m_active].m_buffer != buffer)
            glBindBuffer(m_target, buffer->name());
        
        if(layout.integer_flag == true)
            glVertexAttribIPointer(m_index, layout.size, layout.type, layout.stride, (GLvoid *) layout.offset);
        else
            glVertexAttribPointer(m_index, layout.size, layout.type, layout.normalize_flag ? GL_TRUE : GL_FALSE, layout.stride, (GLvoid *) layout.offset);
        
    #ifdef VERBOSE
        if((attribute.isIntegerType() == true) && (layout.integer_flag == false))
            printf("setBufferState(): implicit cast: integer shader attribute, float buffer\n");
    #endif
    }
    
    if(m_layout.divisor != layout.divisor)
        glVertexAttribDivisor(m_index, layout.divisor);
    
    if(m_buffer == NULL)        // reactiver 
        glEnableVertexAttribArray(m_index);
    
    m_buffer= buffer;
    m_layout= layout;
    ActiveAttributes[m_active].m_buffer= buffer;
    ActiveAttributes[m_active].m_layout= layout;        // ?
    return *this;
}

BufferState& BufferState::setBufferState( GLIndexBuffer *buffer, const BufferLayout& layout )
{
    assert(buffer != NULL);
    assert(m_target == GL_ELEMENT_ARRAY_BUFFER);
    
    if(ActiveAttributes[m_active].m_buffer != buffer)
        glBindBuffer(m_target, buffer->name());

    m_buffer= buffer;
    m_layout= layout;
    ActiveAttributes[m_active].m_buffer= buffer;
    ActiveAttributes[m_active].m_layout= layout;
    return *this;
}

BufferState& BufferState::resetVertexBuffer( )
{
    assert(m_target == GL_ARRAY_BUFFER);
    
    //~ if(m_layout.divisor != 0)
        //~ glVertexAttribDivisor(m_index, 0);
    glDisableVertexAttribArray(m_index);
    
    m_buffer= NULL;
    //~ m_layout.divisor= 0;
    return *this;
}

BufferState& BufferState::resetIndexBuffer( )
{
    assert(m_target == GL_ELEMENT_ARRAY_BUFFER);
    
    glBindBuffer(m_target, 0);
    m_buffer= NULL;
    return *this;
}


BufferState& BufferState::reset( )
{
    // dispatch ? ou virtual reset( ) ?
    return *this;
}

BufferState& BufferState::setBuffer( GLBuffer *buffer )
{
    assert(buffer != NULL);
    
    if(m_buffer != buffer)
        glBindBuffer(m_target, buffer->name());
    
    m_buffer= buffer;
    return *this;
}

BufferState& BufferState::setLayout( const BufferLayout& layout )
{
    //~ if(ActiveAttributeBuffer != m_buffer)
        //~ glBindBuffer(GL_ARRAY_BUFFER, (m_buffer != NULL) ? m_buffer->name() : 0);
    //~ if(m_layout.integer_flag == true)
        //~ glVertexAttribIPointer(m_index, layout.size, layout.type, layout.stride, (GLvoid *) layout.offset);
    //~ else
        //~ glVertexAttribPointer(m_index, layout.size, layout.type, layout.normalize_flag, layout.stride, (GLvoid *) layout.offset);
    //~ if(ActiveAttributes[m_index].m_layout.divisor != layout.divisor)
        //~ glVertexAttribDivisor(m_index, layout.divisor);
    //~ glEnableVertexAttribArray(m_index);

    //~ ActiveAttributeBuffer= m_buffer;
    //~ m_layout= layout;
    return *this;
}


int DrawArrays( const GLenum mode, const int first, const unsigned int count )
{
    // verifier que tous les attributs du shader sont associes a un buffer.
    glDrawArrays(mode, first, count);
    return 0;
}

int DrawArraysInstanced( const GLenum mode, const int first, const unsigned int count, const unsigned int instance_count )
{
    // verifier que tous les attributs du shader sont associes a un buffer.
    glDrawArraysInstanced(mode, first, count, instance_count);
    return 0;
}

int DrawElements( const GLenum mode, const unsigned int count, const int base )
{
    // verifier que tous les attributs du shader sont associes a un buffer.
    const int n= (count > 0) ? count : Attributes[INDEX].getBuffer()->count();
    if(base > 0)
        glDrawElementsBaseVertex( mode, n, Attributes[INDEX].getLayout().type, (GLvoid *) Attributes[INDEX].getLayout().offset, base );
    else
        glDrawElements( mode, n, Attributes[INDEX].getLayout().type, (GLvoid *) Attributes[INDEX].getLayout().offset );
    return 0;
}

int DrawElementsInstanced(const GLenum mode, const unsigned int instance_count, const unsigned int count, const int base )
{
    // verifier que tous les attributs du shader sont associes a un buffer.
    const int n= (count > 0) ? count : Attributes[INDEX].getBuffer()->count();
    if(base > 0)
        glDrawElementsInstancedBaseVertex( mode, n, Attributes[INDEX].getLayout().type, (GLvoid *) Attributes[INDEX].getLayout().offset, instance_count, base );
    else
        glDrawElementsInstanced( mode, n, Attributes[INDEX].getLayout().type, (GLvoid *) Attributes[INDEX].getLayout().offset, instance_count );
    return 0;
}

}
