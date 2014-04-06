#include "glincludes.h"
#include "Geometry.h"
#include "Camera.h"

void onGlfwError(int error, const char* description);
void onResizeWindow(GLFWwindow* window, int w, int h);
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleKeys(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);

//global variables ftw
extern gl::Billboard* bb;
extern gl::Camera camera;
extern glm::mat4 projectionMatrix;