#include "callbacks.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#include <Windows.h> //to toggle fullscreen
#endif

int shader=0;
int wireframe;

void onGlfwError(int error, const char* description){
	std::cout << "GLFW error " << error << ": " << description << std::endl;
}

void onResizeWindow(GLFWwindow* window, int w, int h){
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	float aspect = static_cast<float>(w)/static_cast<float>(h);
	projectionMatrix = glm::perspective(45.f,aspect,-1.f,1.f);
	camera.SetAspectRatio(aspect);
	hdr.setup(glm::ivec2(w, h));
}
using glm::vec3;
//menu keys
#ifdef _DEBUG
bool fullscreen = false;
#else
bool fullscreen = true;
#endif
bool cursorGrabbed = false;
void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE){
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if(key == GLFW_KEY_C && action == GLFW_PRESS){
		if(cursorGrabbed){
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		} else {
			glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
		}
		cursorGrabbed = !cursorGrabbed;
	}
	if(key == GLFW_KEY_Y && action == GLFW_PRESS){
		if(wireframe == 0){
			//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			wireframe = 1;
		} else {
			//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
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
	if(key == GLFW_KEY_F && action == GLFW_PRESS){
		fullscreen = !fullscreen;
#ifdef _WIN32
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		LONG lExStyle, lStyle;
		HWND hwnd = glfwGetWin32Window(window);
		lStyle = GetWindowLong(hwnd,GWL_STYLE);
		lExStyle = GetWindowLong(hwnd,GWL_EXSTYLE);
		if(fullscreen){
			SetWindowLong(hwnd, GWL_STYLE,
						lStyle & ~(WS_CAPTION | WS_THICKFRAME));
			SetWindowLong(hwnd, GWL_EXSTYLE,
						lExStyle & ~(WS_EX_DLGMODALFRAME |
						WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

			SetWindowPos(hwnd, NULL, 0, 0,
						mode->width, mode->height,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}else{
			SetWindowLong(hwnd, GWL_STYLE, lStyle | WS_CAPTION | WS_THICKFRAME);
			SetWindowLong(hwnd, GWL_EXSTYLE, lExStyle | WS_EX_DLGMODALFRAME |
						WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
			glfwSetWindowSize(window,1280,800);
			glfwSetWindowPos(window,(mode->width-1280)/2,(mode->height-800)/2);
		}
#endif
	}
	if(key == GLFW_KEY_V && action == GLFW_PRESS){
		static int interval;
		interval = interval==0?1:0;
		glfwSwapInterval(interval);
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
	if(glfwGetKey(window,GLFW_KEY_Q) == GLFW_PRESS){
		camera.RollCounterClockwise(3.14159f/36.f);
	}
	if(glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS){
		camera.RollClockwise(3.14159f/36.f);
	}
}

double oldMouseX, oldMouseY;
bool hasmouse = false;
void onCursorMoved(GLFWwindow* window, double xpos, double ypos){
	if(hasmouse && cursorGrabbed){
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
	hasmouse = cursorGrabbed;
}