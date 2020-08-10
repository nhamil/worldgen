#pragma once 

#include "Fjord/Graphics/GraphicsAPI.h" 

#include <GL/glew.h> 

#include <iostream> 

namespace Fjord 
{
    const char* GetOpenGLErrorString(GLenum err); 
}

#define GLCALL(x) { \
    x; \
    int glerror = glGetError(); \
    if (glerror != GL_NO_ERROR) FJ_EFLOG(Error, "OpenGL Error (%d): %s", glerror, GetOpenGLErrorString(glerror)); }
