//
// nvWidgets.h - User Interface library
//
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef NV_WIDGETS_H
#define NV_WIDGETS_H

#include <time.h> // time_t

#ifdef WIN32
#ifdef NVWIDGETS_EXPORTS
#define NVSDKENTRY __declspec(dllexport)
#else
#define NVSDKENTRY __declspec(dllimport)
#endif
#else
#define NVSDKENTRY
#endif

namespace nv {

struct NVSDKENTRY Point {
    Point() : x(0), y(0) {}
    Point(int ix, int iy) : x(ix), y(iy) {}
    Point(const Point & p) : x(p.x), y(p.y) {}

    const Point& operator= (const Point & p) { this->x = p.x; this->y = p.y; return *this; }

    int x, y;
};

struct NVSDKENTRY Rect {
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(const Point & p) : x(p.x), y(p.y), w(0), h(0) {}
    Rect(int ix, int iy, int iw = 0, int ih = 0) : x(ix), y(iy), w(iw), h(ih) {}
    Rect(const Rect & r) : x(r.x), y(r.y), w(r.w), h(r.h) {}

    const Rect& operator= (const Rect & r) { this->x = r.x; this->y = r.y; this->w = r.w; this->h = r.h; return *this; }

    int x, y;
    int w, h;

    static const Rect null;
};

enum ButtonFlags {
    ButtonFlags_Off = 0x0,
    ButtonFlags_On = 0x1,
    ButtonFlags_Begin = 0x2,
    ButtonFlags_End = 0x4,
    ButtonFlags_Shift = 0x8,
    ButtonFlags_Alt = 0x10,
    ButtonFlags_Ctrl = 0x20,
};

struct ButtonState {
    int state;
    time_t time;
    Point cursor;
};

// An enum to identify the mouse buttons
enum MouseButton {
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
};

// An enum to identify the special key buttons not translated with ASCII codes
enum Key {
    Key_F1 = 128,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,

    Key_Left,
    Key_Up,
    Key_Right,
    Key_Down,
    Key_PageUp,
    Key_PageDown,
    Key_Home,
    Key_End,
    Key_Insert,
    Key_Tab
};

// The various flags to modify the behavior of the groups
enum GroupFlags {
    // Layout behavior flags
    GroupFlags_LayoutNone = 0x01,
    GroupFlags_LayoutVertical = 0x02,
    GroupFlags_LayoutHorizontal = 0x04,
    GroupFlags_LayoutMask = 0x07,
    GroupFlags_LayoutXMask = 0xffff ^ GroupFlags_LayoutMask,

    // Alignment flags for the widgets inserted in the group
    GroupFlags_AlignLeft = 0x10,
    GroupFlags_AlignRight = 0x20,
    GroupFlags_AlignTop = 0x40,
    GroupFlags_AlignBottom = 0x80,
    GroupFlags_AlignMask = 0xf0,
    GroupFlags_AlignXMask = 0xffff ^ GroupFlags_AlignMask,

    // Start flags defining the starting origin of the group
    GroupFlags_StartLeft = 0x100,
    GroupFlags_StartRight = 0x200,
    GroupFlags_StartTop = 0x400,
    GroupFlags_StartBottom = 0x800,
    GroupFlags_StartMask = 0xf00,
    GroupFlags_StartXMask = 0xffff ^ GroupFlags_StartMask,

    // Optional flags
    GroupFlags_LayoutForce = 0x8000,
    GroupFlags_LayoutDefault = 0x4000,
    GroupFlags_LayoutNoMargin = 0x2000,
    GroupFlags_LayoutNoSpace = 0x1000,

    // Predefined configurations
    GroupFlags_GrowRightFromBottom   = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartBottom | GroupFlags_AlignBottom ,
    GroupFlags_GrowRightFromTop      = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartTop    | GroupFlags_AlignTop ,
    GroupFlags_GrowLeftFromBottom    = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartBottom | GroupFlags_AlignBottom,
    GroupFlags_GrowLeftFromTop       = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartTop    | GroupFlags_AlignTop,
    GroupFlags_GrowUpFromLeft        = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
    GroupFlags_GrowUpFromRight       = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartRight  | GroupFlags_AlignRight ,
    GroupFlags_GrowDownFromLeft      = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
    GroupFlags_GrowDownFromRight     = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartRight  | GroupFlags_AlignRight ,

    GroupFlags_LayoutDefaultFallback = GroupFlags_GrowDownFromLeft,
};

struct Group {
    Rect bounds;  // anchor point + width and height of the region
    int flags;    // group behavior
    int margin;   // border
    int space;    // interior
};

//*************************************************************************
// UIPainter
class NVSDKENTRY UIPainter {
public:
    UIPainter() {}

    virtual void begin( const Rect & /*window*/ ) { init(); }
    virtual void end() {}

    // These methods should be called between begin/end

    virtual void drawFrame(const Rect & r, int margin, int style) = 0;

    virtual Rect getLabelRect(const Rect & r, const char * text, Rect & rt, int& nbLines) const = 0;
    virtual void drawLabel(const Rect & r, const char * text, const Rect & rt, const int& nbLines, bool isHover, int style) = 0;

    virtual Rect getButtonRect(const Rect & r, const char * text, Rect & rt) const = 0;
    virtual void drawButton(const Rect & r, const char * text, const Rect & rt, bool isDown, bool isHover, bool isFocus, int style) = 0;

    virtual Rect getCheckRect(const Rect & r, const char * text, Rect & rt, Rect & rc) const = 0;
    virtual void drawCheckButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isChecked, bool isHover, bool isFocus, int style) = 0;

    virtual Rect getRadioRect(const Rect & r, const char * text, Rect & rt, Rect & rr) const = 0;
    virtual void drawRadioButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isOn, bool isHover, bool isFocus, int style) = 0;

    virtual Rect getHorizontalSliderRect(const Rect & r, Rect& rs, float v, Rect& rc) const = 0;
    virtual void drawHorizontalSlider(const Rect & r, Rect& rs, float v, Rect& rc, bool isHover, int style) = 0;

    virtual Rect getItemRect(const Rect & r, const char * text, Rect & rt) const = 0;
    virtual void drawListItem(const Rect & r, const char * text, const Rect & rt, bool isSelected, bool isHover, int style) = 0;

    virtual Rect getListRect(const Rect & r, int numOptions, const char * options[], Rect& ri, Rect & rt) const = 0;
    virtual void drawListBox(const Rect & r, int numOptions, const char * options[], const Rect& ri, const Rect & rt, int selected, int hovered, int style) = 0;

    virtual Rect getComboRect(const Rect & r, int numOptions, const char * options[], int selected, Rect& rt, Rect& ra) const = 0;
    virtual Rect getComboOptionsRect(const Rect & rCombo, int numOptions, const char * options[], Rect& ri, Rect & rit) const = 0;
    virtual void drawComboBox(const Rect & rect, int numOptions, const char * options[], const Rect & rt, const Rect & ra, int selected, bool isHover, bool isFocus, int style) = 0;
    virtual void drawComboOptions(const Rect & rect, int numOptions, const char * options[], const Rect& ri, const Rect & rit, int selected, int hovered, bool isHover, bool isFocus, int style) = 0;

    virtual Rect getLineEditRect(const Rect & r, const char * text, Rect & rt) const = 0;
    virtual void drawLineEdit(const Rect & r, const char * text, const Rect & rt, int caretPos, bool isSelected, bool isHover, int style) = 0;

    virtual Rect getPanelRect(const Rect & r, const char * text, Rect& rt, Rect& ra) const = 0;
    virtual void drawPanel(const Rect & rect, const char * text, const Rect & rt, const Rect & ra, bool isUnfold, bool isHover, bool isFocus, int style) = 0;

    virtual Rect getTextureViewRect(const Rect & rect, Rect& rt) const = 0;
    virtual void drawTextureView(const Rect & rect, const void* texID, const Rect& rt, const Rect & rz, int mipLevel,
            float texelScale, float texelOffset, int r, int g, int b, int a,
            int style) = 0;

    // Eval widget dimensions
    virtual int getWidgetMargin() const = 0;
    virtual int getWidgetSpace() const = 0;
    virtual int getAutoWidth() const = 0;
    virtual int getAutoHeight() const = 0;
    
    virtual int getCanvasMargin() const = 0;
    virtual int getCanvasSpace() const = 0;
    virtual int getFontHeight() const = 0;
    virtual int getTextLineWidth(const char * text) const = 0;
    virtual int getTextSize(const char * text, int& nbLines) const = 0;
    virtual int getPickedCharNb(const char * text, const Point& at) const = 0;

    virtual void drawDebugRect(const Rect & r) = 0;

protected:

    void init() {}
};


class NVSDKENTRY UIContext {
public:
    UIContext( UIPainter& painter );

    //
    // UI method for processing window size events
    //////////////////////////////////////////////////////////////////
    void reshape(int w, int h);

    //
    // Check if the UI is currently on Focus
    //////////////////////////////////////////////////////////////////
    bool isOnFocus() const { return m_uiOnFocus; }

    //
    // UI method for processing mouse events
    //////////////////////////////////////////////////////////////////
    void mouse(int button, int state, int modifier, int x, int y);
    void mouse(int button, int state, int x, int y) { mouse( button, state, 0, x, y); }

    //
    // UI method for processing mouse motion events
    //////////////////////////////////////////////////////////////////
    void mouseMotion(int x, int y);

    //
    // UI method for processing key events
    //////////////////////////////////////////////////////////////////
    void keyboard(unsigned char k, int x, int y);

    //
    // UI method for entering UI processing mode
    //
    //  This function must be used to begin the UI processing
    //////////////////////////////////////////////////////////////////
    void begin();

    //
    // UI method for leaving UI processing mode
    //
    //  This function must be used to end the UI processing
    //////////////////////////////////////////////////////////////////
    void end();

    ////////////////////////////////////////////////////////////////////////////
    //
    // UI element processing
    //
    // The following methods provide the interface for rendering and querying
    // UI objects. These methods must be called between begin/end.
    ////////////////////////////////////////////////////////////////////////////

    //
    // UI method for drawing a static text label
    // The label display a non interactive text.
    // The text can have multiple lines
    //
    // rect - optionally provides a location and size for the label
    // text - Text to display for the label (can have several lines)
    //////////////////////////////////////////////////////////////////
    void doLabel(const Rect & rect, const char * text, int style = 0);

    //
    // UI method for rendering and processing a push button
    //
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // state -  whether the button is depressed
    //          if state is NULL, the buttoin behave like a touch button
    //          else, the button behave like a toggle button
    // style - optional style flag to modify the look
    //
    // @return  True if the button'state changed
    //////////////////////////////////////////////////////////////////
    bool doButton(const Rect & rect, const char * text, bool * state = NULL, int style = 0);

    //
    // UI method for rendering and processing a check button
    // Check button behaves similarly as a toggle button
    // used to display and edit a bool property.
    //
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // state -  whether the check button is checked or not
    //          if state is NULL, the buttoin behave like if a touch button unchecked
    // style - optional style flag to modify the look
    //
    // @return  True if the check button'state changed
    //////////////////////////////////////////////////////////////////
    bool doCheckButton(const Rect & rect, const char * text, bool * state, int style = 0);

    //
    // UI method for rendering and processing a radio button
    // Radio buttons are typically used in groups to diplay and edit
    // the possible reference values taken by an int value.
    //
    // One radio button is representing a possible reference value taken by the current value.
    // It is displaying a boolean state true if the current value is equal to the reference value.
    //
    // reference - The reference int value represented by this radio button.
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // value -  The value parameter compared to the reference value parameter.
    //          if value is NULL, the radio button is off
    // style - optional style flag to modify the look
    //
    // @return  True if the radio button's value changed
    //////////////////////////////////////////////////////////////////
    bool doRadioButton(int reference, const Rect & r, const char * text, int * value, int style = 0);

    //
    // UI method for rendering and processing a horizontal slider
    // Horizontal slider is used to edit and display a scalar value in the specified range [min, max].
    //
    // rect - optionally provides a location and size for the widget
    // min - min bound of the varying range of the value
    // max - max bound of the varying range of the value
    // value -  the value edited byt the widget
    //          if value is NULL, the value is set to min
    // style - optional style flag to modify the look
    //
    // @return  True if the slider's value changed
    //////////////////////////////////////////////////////////////////
    bool doHorizontalSlider(const Rect & rect, float min, float max, float * value, int style = 0);

    bool doListItem(int index, const Rect & rect, const char * text, int * selected, int style = 0);
    bool doListBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);
    bool doComboBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);

    bool doLineEdit(const Rect & rect, char * text, int maxTextLength, int * nbCharsReturned, int style = 0);



    void beginGroup(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null);
    void endGroup();

    void beginFrame(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null, int style = 0);
    void endFrame();

    bool beginPanel(Rect & rect, const char * text, bool * isUnfold, int groupFlags = GroupFlags_LayoutDefault, int style = 0);
    void endPanel();

    int getGroupWidth() { return m_groupStack[m_groupIndex].bounds.w; }
    int getGroupHeight() { return m_groupStack[m_groupIndex].bounds.h; }

    int getCursorX() { return m_currentCursor.x;}
    int getCursorY() { return m_currentCursor.y;}

    const ButtonState& getMouseState( int button) { return m_mouseButton[button]; }

    //
    // UI method for drawing a texture view
    // Several parameters control the equation used to display the texel
    //      texel = texture2DFetch(...);
    //      pixel = texelSwizzling( texel * texelScale + texelOffset );
    //
    // rect - provides a location and size for the texture view
    // texID - texture identifier (Graphics API specific)
    // zoomRect - rectangle area of the texture displayed
    // mipLevel - mip Level of the texture displayed
    // texelScale - scale applyed to the texel fetch
    // texelOffset - offset applyed to the texel after scale
    // texelSwizzling - swizzle applyed to the texel (after scale and offset)
    // style - optional style flag to modify the look
    //////////////////////////////////////////////////////////////////
    void doTextureView( const Rect & rect, const void* texID, Rect & zoomRect, int mipLevel = -1,
            float texelScale = 1.0f, float texelOffset = 0.0f,
            int red = 0, int green = 1, int blue = 2, int alpha = 3,
            int style = 0);

protected:
    UIPainter* getPainter() { return m_painter; }

    const Rect  & window() const { return m_window; }

private:
    void setCursor(int x, int y);

    static bool overlap(const Rect & rect, const Point & p);

    bool hasFocus(const Rect & rect);
    bool isHover(const Rect & rect);

    Rect placeRect(const Rect & r);

private:
    UIPainter * m_painter;

    int m_groupIndex;
    Group m_groupStack[64];

    Rect m_window;

    Point m_currentCursor;
    ButtonState m_mouseButton[3];
    unsigned char m_keyBuffer[32];
    int m_nbKeys;

    int m_focusCaretPos;
    Point m_focusPoint;
    bool m_twoStepFocus;
    bool m_uiOnFocus;
};

};


#endif  // NV_WIDGETS_H
