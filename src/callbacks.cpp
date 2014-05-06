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
	camera.SetAspectRatio(aspect);
}
using glm::vec3;
//menu keys
#ifdef _DEBUG
bool fullscreen = false;
#else
bool fullscreen = true;
#endif
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE){
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if(key == GLFW_KEY_C){
		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
	}
	if(key == GLFW_KEY_Y && action == GLFW_PRESS){
		static int wireframe;
		if(wireframe == 0){
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			wireframe = 1;
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			wireframe = 0;
		}
	}
	if(key == GLFW_KEY_T && action == GLFW_PRESS){
		static int culling;
		if(culling == 0){
			glDisable(GL_CULL_FACE);
			culling = 1;
		} else {
			glEnable(GL_CULL_FACE);
			culling = 0;
		}
	}
}
//controls
void handleKeys(GLFWwindow* window){
	if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
		//camera.moveLocal(vec3(0.f,0.f,0.01f));
		camera.MoveForward(0.02f);
	}
	if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
		//camera.moveLocal(vec3(0.01f,0.f,0.0f));
		camera.StrafeLeft(0.02f);
	}
	if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
		//camera.moveLocal(vec3(0.f,0.f,-0.01f));
		camera.MoveBackward(0.02f);
	}
	if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
		//camera.moveLocal(vec3(-0.01f,0.f,0.f));
		camera.StrafeRight(0.02f);
	}
	if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		//camera.moveLocal(vec3(0.f,-0.01f,0.f));
		camera.MoveUp(0.21f);
	}
	if(glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		//camera.moveLocal(vec3(0.f,0.01f,0.f));
		camera.MoveDown(0.02f);
	}
}

double oldMouseX, oldMouseY;
bool hasmouse = false;
void onCursorMoved(GLFWwindow* window, double xpos, double ypos){
	if(hasmouse){
		double dx = oldMouseX-xpos;
		double dy = oldMouseY-ypos;

		//camera.rotate(-dy,glm::inverse(camera.rot())*glm::vec3(1.f,0.f,0.f));
		camera.PitchUp((float)dy*0.001f);
		//camera.rotate(dx,vec3(0.,1.,0.));
		//camera.RotateAround(vec3(0.f,1.f,1.f),-dx*0.01);
		camera.YawLeft((float)dx*0.001f);
	}
	oldMouseX = xpos;
	oldMouseY = ypos;
	hasmouse = true;
}