 
#ifndef _TP_ATTRIBUTES_H
#define _TP_ATTRIBUTES_H

#include "GL/GLPlatform.h"
#include "GL/TPProgramName.h"

namespace gk {

enum
{
    INDEX= 0,
    
    ATTRIBUTE,
    ATTRIBUTE0,
    ATTRIBUTE1,
    ATTRIBUTE2,
    ATTRIBUTE3,
    ATTRIBUTE4,
    ATTRIBUTE5,
    ATTRIBUTE6,
    ATTRIBUTE7,
    
    FEEDBACK,
    FEEDBACK0,
    FEEDBACK1,
    FEEDBACK2,
    FEEDBACK3,
    
    UNIFORM,
    UNIFORM0,
    UNIFORM1,
    UNIFORM2,
    UNIFORM3,
    UNIFORM4,
    UNIFORM5,
    UNIFORM6,
    UNIFORM7,
    
    INDIRECT,
    
    BUFFER_STATE_LAST
};


class GLBuffer;
class GLAttributeBuffer;
class GLIndexBuffer;

struct BufferLayout
{
    int size;
    GLenum type;
    unsigned long int stride;
    unsigned long int offset;
    int divisor;
    bool normalize_flag;
    bool integer_flag;
    
    enum
    {
        NORMALIZE_BIT= 1,
        INTEGER_BIT= 2
    };
    
    BufferLayout( )
        :
        size(0),
        type(GL_NONE),
        stride(0),
        offset(0),
        divisor(0),
        normalize_flag(false),
        integer_flag(false)
    {}
    
    BufferLayout( const int _size, const GLenum _type, const unsigned long int _stride= 0, const unsigned long int _offset= 0, 
        const int _divisor= 0, const unsigned int flags= 0 )
        :
        size(_size),
        type(_type),
        stride(_stride),
        offset(_offset),
        divisor(_divisor),
        normalize_flag((flags & NORMALIZE_BIT)),
        integer_flag((flags & INTEGER_BIT))
    {}
    
    bool operator==( const BufferLayout& b )
    {
        if(stride != b.stride)
            return false;
        if(offset != b.offset)
            return false;
        if(size != b.size || type != b.type)
            return false;
        if(divisor != b.divisor)
            return false;
        
        return true;
    }
    
    bool operator!= ( const BufferLayout& b )
    {
        return !operator==(b);
    }
};

class BufferState // : public IState
{
    GLBuffer *m_buffer;
    BufferLayout m_layout;
    GLenum m_target;    // GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_FEEDBACK_BUFFER, etc.
    int m_index;        // indice pour les semantiques indexees
    int m_active;       // indice de l'etat / buffer actif pour la semantique.

    BufferState( const BufferState& );
    BufferState( const GLenum target, const int active, const int index= -1)
        :
        m_buffer(NULL),
        m_layout(),
        m_target(target),
        m_index(index),
        m_active(active)
    {}
    
public:
    BufferState( )
        :
        m_buffer(NULL),
        m_layout(),
        m_index(-1)
    {}
    
    ~BufferState( ) 
    {}

    BufferState& operator=( const BufferState& b )
    {
        m_buffer= b.m_buffer;
        m_layout= b.m_layout;
        m_target= b.m_target;
        m_index= b.m_index;
        m_active= b.m_active;
        return *this;
    }

    BufferState& setBufferState( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout );
    BufferState& setBufferState( GLIndexBuffer *buffer, const BufferLayout& layout );
    
    BufferState& resetVertexBuffer( );
    BufferState& resetIndexBuffer( );

    BufferState& reset( );
    
    BufferState& setBuffer( GLBuffer *buffer );
    
    GLBuffer *getBuffer( )
    {
        return m_buffer;
    }
    
    const GLBuffer *getBuffer( ) const
    {
        return m_buffer;
    }
    
    BufferState& setLayout( const BufferLayout& layout );

    BufferLayout& getLayout( )
    {
        return m_layout;
    }

    const BufferLayout& getLayout( ) const
    {
        return m_layout;
    }
    
    GLenum target( ) const
    {
        return m_target;
    }
    
    int index( ) const
    {
        return m_index;
    }
    
    int active( ) const
    {
        return m_active;
    }
    
    static int init( );
};

extern BufferState Attributes[BUFFER_STATE_LAST];
extern BufferState ActiveAttributes[BUFFER_STATE_LAST];
extern int AttributeCount;
extern bool AttributesDirty;

BufferState& setVertexBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout );
BufferState& setVertexBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride= 0, const unsigned int offset= 0, const unsigned int flags= 0 );
BufferState& resetVertexBuffer( const ProgramAttribute& attribute );

BufferState& setInstanceBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, const BufferLayout& layout );
BufferState& setInstanceBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride= 0, const unsigned int offset= 0, const unsigned int flags= 0, const int divisor= 1 );
BufferState& resetInstanceBuffer( const ProgramAttribute& attribute );

//! integer attributes (vertex buffer)
inline
BufferState& setIntegerVertexBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride= 0, const unsigned int offset= 0, const unsigned int flags= 0 )
{
    return setVertexBuffer(attribute, buffer, size, type, stride, offset, flags | BufferLayout::INTEGER_BIT);
}

//! integer attributes (instance buffer)
inline
BufferState& setIntegerInstanceBuffer( const ProgramAttribute& attribute, GLAttributeBuffer *buffer, 
    const int size, const GLenum type, const unsigned int stride= 0, const unsigned int offset= 0, const unsigned int flags= 0, const int divisor= 1 )
{
    return setInstanceBuffer(attribute, buffer, size, type, stride, offset, flags | BufferLayout::INTEGER_BIT, divisor);
}

BufferState& setIndexBuffer( GLIndexBuffer *buffer, const BufferLayout& layout );
BufferState& setIndexBuffer( GLIndexBuffer *buffer, const GLenum type= GL_UNSIGNED_INT, const unsigned int offset= 0 );
BufferState& resetIndexBuffer( );


int DrawArrays( const GLenum mode, const int first, const unsigned int count );
int DrawArraysInstanced( const GLenum mode, const int first, const unsigned int count, const unsigned int instance_count= 1 );
int DrawElements( const GLenum mode, const unsigned int count= 0, const int base= 0 );
int DrawElementsInstanced(const GLenum mode, const unsigned int instance_count= 1, const unsigned int count= 0, const int base= 0 );

}

#endif
