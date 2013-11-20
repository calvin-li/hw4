// Source: https://github.com/mortennobel/QtOpenGL3.2Core

#ifndef GLHEADER_H
#define GLHEADER_H

#include <iostream>
#include <string>
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <GL/glew.h>
#elif defined(Q_OS_MAC)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

void glError(const char *file, int line);

void glSetup();
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define glCheckError() glError(__FILE__,__LINE__)

#endif // GLHEADER_H
