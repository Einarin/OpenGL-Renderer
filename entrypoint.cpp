#include "glincludes.h"
#include "callbacks.h"
#include <iostream>
#include "Texture.h"
#include "Geometry.h"
#include "Shader.h"
#include "Model.h"

using namespace std;
using namespace gl;

//global variables ftw
Billboard* bb;

int main(int argc, char* argv[])
{
	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowSize(800,600);
	//glutCreateWindow("Game");
	GLFWwindow* window;

	glfwSetErrorCallback(onGlfwError);

	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
	if (!window)
	{
	glfwTerminate();
	exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "glewInit failed, aborting." << endl;
		exit (1);
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;

	//set glfw callbacks
	glfwSetKeyCallback(window, onKeyPressed);
	glfwSetWindowSizeCallback(window, onResizeWindow);

	glClearColor(0.0f,0.1f,0.3f,1.0f);

	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	//hello world setup
	
	Model cube("assets/shuttle.3ds");
	cube.init();
	cube.download();

	bb = new Billboard(-1.f,-1.f,2.f,2.f);
	bb->init();
	bb->download();
	checkGlError("geometry");
	Ref<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	vs->compile("attribute vec4 in_Position;\n"
"attribute vec4 in_texCoords;\n"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	texCoords = in_texCoords;\n"
"	gl_Position = in_Position/2.0; \n"
"}\n");
	Ref<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	/*fs->compile("uniform sampler2D framedata;"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	vec4 sample = texture2D(framedata,texCoords.st);\n"
"	gl_FragColor = vec4(texCoords.s,texCoords.t,1.-texCoords.s,1.0);\n"
"}\n");*/
	fs->compileFromFile("lava.frag");
	Ref<Shader> shader = Shader::Allocate();
	Ref<Shader> shaderCopy(shader);
	shader->addAttrib("in_Position",0);
	shader->addAttrib("in_texCoords",1);
	shader->attachStage(vs);
	shader->attachStage(fs);
	shader->link();
	shader->bind();
	checkGlError("shader");
	glUniform1i(shader->getUniformLocation("framedata"), 0);
	TextureManager* texMan = TextureManager::Instance();
	TexRef tex = texMan->texFromFile("lego.png");
	checkGlError("texFromFile");
	glActiveTexture(GL_TEXTURE0);
	tex->bind();
	checkGlError("bindTexture");
	float i = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_UP)){
			bb->moveRel(0.f,.01f);
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_DOWN)){
			bb->moveRel(0.f,-.01f);
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_LEFT)){
			bb->moveRel(-.01f,0.f);
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_RIGHT)){
			bb->moveRel(.01f,0.f);
		}
		glClear(GL_COLOR_BUFFER_BIT);
		cube.draw();
		glUniform1f(shader->getUniformLocation("time"),i);
		i+=0.001f;
		bb->draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}