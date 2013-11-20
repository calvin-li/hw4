// Source: https://github.com/mortennobel/QtOpenGL3.2Core

#ifndef CORE3_2_CONTEXT_H
#define CORE3_2_CONTEXT_H
//#include <QtGlobal>
#include <QGLContext>

#if defined(Q_OS_MAC)
#if QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800 // if less or equal to 4.8.5
void* select_3_2_mac_visual(GDHandle handle);
#endif
#endif

struct Core3_2_context : public QGLContext {
    Core3_2_context(const QGLFormat& format, QPaintDevice* device) : QGLContext(format,device) {}
    Core3_2_context(const QGLFormat& format) : QGLContext(format) {}

#if defined(Q_OS_MAC)
#if QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800// if less or equal to 4.8.5
    virtual void* chooseMacVisual(GDHandle handle) {
        return select_3_2_mac_visual(handle);
    }
#endif
#endif
};

#endif // CORE3_2_CONTEXT_H
