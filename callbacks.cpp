#include "callbacks.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

int shader=0;

void onGlfwError(int error, const char* description){
	std::cout << "GLFW error " << error << ": " << description << std::endl;
}

void onResizeWindow(GLFWwindow* window, int w, int h){
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	float aspect = static_cast<float>(w)/static_cast<float>(h);
	projectionMatrix = glm::perspective(45.f,aspect,-1.f,1.f);
}
using glm::vec3;
//menu keys
bool fullscreen = false;
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE){
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if(key == GLFW_KEY_C){
		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
	}
}
//controls
void handleKeys(GLFWwindow* window){
	if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
		camera.moveBy(vec3(0.f,0.f,0.1f));
	}
	if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
		camera.moveBy(vec3(0.1f,0.f,0.f));
	}
	if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
		camera.moveBy(vec3(0.f,0.f,-0.1f));
	}
	if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
		camera.moveBy(vec3(-0.1f,0.f,0.f));
	}
	if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		camera.moveBy(vec3(0.f,-0.1f,0.f));
	}
	if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		camera.moveBy(vec3(0.f,0.1f,0.f));
	}
}

double oldMouseX, oldMouseY;
bool hasmouse = false;
void onCursorMoved(GLFWwindow* window, double xpos, double ypos){
	if(hasmouse){
		double dx = oldMouseX-xpos;
		double dy = oldMouseY-ypos;
		double angle = glm::length(glm::vec3(-dy,dx,0.0));
		glm::vec3 axis = glm::normalize(glm::vec3(-dy,dx,0.0));
		glm::quat q;
		q = glm::rotate(q,angle,axis);
		camera.rotate(q);
		camera.zeroZ();
	}
	oldMouseX = xpos;
	oldMouseY = ypos;
	hasmouse = true;
}