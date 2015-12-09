#pragma once
#include "glincludes.h"
#include "Geometry.h"
#include "Camera.h"
#include "HighDynamicRangeResolve.h"
#include "GBuffer.h"

void onGlfwError(int error, const char* description);
void onResizeWindow(GLFWwindow* window, int w, int h);
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleKeys(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);

//global variables ftw
//from entrypoint.cpp
extern gl::Billboard* bb;
extern gl::Camera camera;
extern glm::mat4 projectionMatrix;
extern glm::mat4 orthoMatrix;
extern int width, height;
extern gl::HighDynamicRangeResolve hdr;
extern gl::GBuffer gBuffer;
//from callbacks.cpp
extern int wireframe;