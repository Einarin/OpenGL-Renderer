#pragma once
#include "GL/glew.h"
#ifdef _WIN32
//#include "GL/wglew.h"
#endif //_WIN32
//#include "GL/freeglut.h"
#include "GlFW/glfw3.h"
#define GLM_SWIZZLE 
#include "glm/glm.hpp"
#include <string>
#ifdef _DEBUG
#define checkGlError(step) checkGlErrorImpl(step,__FILE__,__LINE__)
#else
#define checkGlError(step)
#endif
int checkGlErrorImpl(std::string step,std::string file,int line);