
#ifndef _GKQUERY_MANAGER_H
#define _GKQUERY_MANAGER_H

#include "GLManager.h"
#include "GL/GLQuery.h"

namespace gk {

inline    
GLQuery *createQuery( const GLenum target )
{
    return GLManager<GLQuery>::manager().insert(
        new GLQuery(target) );
}

}


#endif
