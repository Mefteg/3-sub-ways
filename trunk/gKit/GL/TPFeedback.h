
#ifndef _TP_FEEDBACK_H
#define _TP_FEEDBACK_H

#include "GL/GLPlatform.h"
#include "GL/TPBuffer.h"


namespace gk {
    
class FeedbackState
{
    GLAttributeBuffer *m_buffer;
    int m_index;
    
    FeedbackState( const FeedbackState& );
    
public:
    FeedbackState( )
        :
        m_buffer(NULL),
        m_index(-1)
    {}
        
    ~FeedbackState( ) {}
    
    FeedbackState& operator= ( const FeedbackState& b )
    {
        m_buffer= b.m_buffer;
        m_index= b.m_index;
        return *this;
    }
    
    FeedbackState& setFeedbackBuffer( const ProgramFeedback& feedback, GLAttributeBuffer *buffer );
    FeedbackState& reset( );
    
    FeedbackState& setBuffer( GLAttributeBuffer *buffer );

    GLAttributeBuffer *getBuffer( )
    {
        return m_buffer;
    }
    
    const GLAttributeBuffer *getBuffer( ) const
    {
        return m_buffer;
    }
    
    FeedbackState& setIndex( const int index );

    int getIndex( ) const
    {
        return m_index;
    }
};

extern FeedbackState Feedbacks[16];
extern FeedbackState ActiveFeedbacks[16];

FeedbackState& setFeedbackBuffer( const ProgramFeedback& feedback, GLAttributeBuffer *buffer );
FeedbackState& resetFeedbackBuffer( const ProgramFeedback& feedback );

}

#endif
