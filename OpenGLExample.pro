QT += core gui
QT += widgets
QT += opengl

DEFINES += GL_GLEXT_PROTOTYPES

win32:INCLUDEPATH *= "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include"
win32:LIBS *= "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\glew32.lib"

mac {
    eval(QT_MAJOR_VERSION = "4") {
        LIBS += -framework Foundation -framework Cocoa
        OBJECTIVE_SOURCES  +=  core_profile_attributes.mm
    }
}

TARGET = OpenGLExample
TEMPLATE = app

SOURCES += \
    GLDisplay.cpp \
    glheader.cpp \
    ViewerMainWindow.cpp \
    ViewerProg.cpp \
		readObj.cpp \
		readBMP.cpp 

HEADERS  += \
    Core3_2_context.h \
    GLDisplay.h \
    glheader.h \
    ViewerMainWindow.h \
    readObj.h \
    readBMP.h
