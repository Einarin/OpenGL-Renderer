#include "glincludes.h"
#include "Geometry.h"

void onGlfwError(int error, const char* description);
void onResizeWindow(GLFWwindow* window, int w, int h);
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);

//global variables ftw
extern gl::Billboard* bb;