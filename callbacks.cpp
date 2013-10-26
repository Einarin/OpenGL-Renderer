#include "callbacks.h"
#include <iostream>

int shader=0;

void onGlfwError(int error, const char* description){
	std::cout << "GLFW error " << error << ": " << description << std::endl;
}

void onResizeWindow(GLFWwindow* window, int w, int h){
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
}

void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods){
	//std::cout << "key pressed\n";
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}