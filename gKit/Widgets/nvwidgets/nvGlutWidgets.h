//
// nvGlutWidgets
//
//  Adaptor classes to integrate the nvWidgets UI library with the GLUT windowing
// toolkit. The adaptors convert native GLUT UI data to native nvWidgets data. All
// adaptor classes are implemented as in-line code in this header. The adaptor
// defaults to using the standard OpenGL painter implementation.
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NV_GLUT_WIDGETS_H
#define NV_GLUT_WIDGETS_H

#include <algorithm> 

#include "nvGLWidgets.h"
#include "nvGlutTextPainter.h"
#include "nvSdlTextPainter.h"

namespace nv {

class NVSDKENTRY GlutUIContext : public UIContext {
public:

    //
    // Default UI constructor
    //
    //  Creates private OpenGL painter
    //////////////////////////////////////////////////////////////////
    GlutUIContext() :
            UIContext( *(new GLUIPainter( *(new GlutTextPainter() )) )),
            m_ownPainter(true) {
    }

    //
    // Alternate UI constructor
    //
    //  Allows for overriding the standard painter
    //////////////////////////////////////////////////////////////////
    GlutUIContext(UIPainter& painter) :
            UIContext( painter ),
            m_ownPainter(false) {
    }

    //
    // UI  destructor
    //
    //  Destroy painter if it is private
    //////////////////////////////////////////////////////////////////
    ~GlutUIContext() {
        if (m_ownPainter) delete getPainter();
    }

    //
    // One time initialization
    //
    //////////////////////////////////////////////////////////////////
    bool init(int w, int h);

    //
    // UI method for processing GLUT mouse button events
    //
    //  Call this method from the glutMouseFunc callback, the
    // modifier parameter maps to glutGetModifiers.
    //////////////////////////////////////////////////////////////////
    virtual void mouse(int button, int state, int modifier, int x, int y);

    void mouse(int button, int state, int x, int y);

    //
    // UI method for processing key events
    //
    //  Call this method from the glutReshapeFunc callback
    //////////////////////////////////////////////////////////////////
    void specialKeyboard(int k, int x, int y);

private:

    //
    //  Translate non-ascii keys from GLUT to nvWidgets
    //////////////////////////////////////////////////////////////////
    unsigned char translateKey( int k );

    bool m_ownPainter;
};

};



#endif
