
#ifndef _GK_STREAM_QUERY_MANAGER_H
#define _GK_STREAM_QUERY_MANAGER_H

#include "GLManager.h"
#include "GL/GLStreamQuery.h"

namespace gk {

inline    
GLStreamQuery *createStreamQuery( const GLenum target, const unsigned int count )
{
    return GLManager<GLStreamQuery>::manager().insert(
        new GLStreamQuery(target, count) );
}

}


#endif
