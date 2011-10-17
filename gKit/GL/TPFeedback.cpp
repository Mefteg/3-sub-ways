
#include "GL/TPFeedback.h"


namespace gk {
    
FeedbackState Feedbacks[16];
FeedbackState ActiveFeedbacks[16];
FeedbackState DefaultFeedback;
    

FeedbackState& setFeedbackBuffer( const ProgramFeedback& feedback, GLAttributeBuffer *buffer )
{
    if(feedback.index() < 0)
        return DefaultFeedback;
    
    Feedbacks[feedback.index()]= ActiveFeedbacks[feedback.index()].setFeedbackBuffer(feedback, buffer);
    return Feedbacks[feedback.index()];
}

FeedbackState& resetFeedbackBuffer( const ProgramFeedback& feedback )
{
    if(feedback.index() < 0)
        return DefaultFeedback;
    
    Feedbacks[feedback.index()]= ActiveFeedbacks[feedback.index()].reset();
    return Feedbacks[feedback.index()];
}


FeedbackState& FeedbackState::setFeedbackBuffer( const ProgramFeedback& feedback, GLAttributeBuffer *buffer )
{
    if(m_buffer != buffer)
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, feedback.index(), (buffer != NULL) ? buffer->name() : 0);
    
    m_index= feedback.index();
    m_buffer= buffer;
    return *this;
}

FeedbackState& FeedbackState::reset( )
{
    if(m_index != -1)
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, m_index, 0);
    
    m_buffer= NULL;
    m_index= -1;
    return *this;
}

FeedbackState& FeedbackState::setBuffer( GLAttributeBuffer *buffer )
{
    if(m_buffer != buffer)
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, m_index, (buffer != NULL) ? buffer->name() : 0);
    m_buffer= buffer;
    return *this;
}
    
FeedbackState& FeedbackState::setIndex( const int index )
{
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, (m_buffer != NULL) ? m_buffer->name() : 0);
    m_index= index;
    return *this;
}


}
