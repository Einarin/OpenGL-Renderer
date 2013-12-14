#include "glincludes.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "callbacks.h"
#include <iostream>
#include "Texture.h"
#include "Geometry.h"
#include "Shader.h"
#include "Model.h"

#include "windows.h" //for debugbreak only!

using namespace std;
using namespace gl;
using namespace glm;

//global variables ftw
Billboard* bb;
Camera camera;
glm::mat4 projectionMatrix;

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
	window = glfwCreateWindow(1000, 1000, "Game", NULL, NULL);
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
	//controls handled by main loop instead
	glfwSetKeyCallback(window, onKeyPressed);
	glfwSetWindowSizeCallback(window, onResizeWindow);
	glfwSetCursorPosCallback(window, onCursorMoved);
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_DEPTH_TEST);

	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		projectionMatrix = glm::perspective(90.f,static_cast<float>(width)/static_cast<float>(height),0.1f,10000.f);
	}

	//Camera setup
	camera = Camera(vec3(0,1,5),vec3(0,0,0),vec3(0,1,0));

	//hello world setup
	
	//Model cube("assets/shuttle.3ds");
	//Sphere cube(32,vec2(0.0,0.0),vec2(1.0));
	Cube cube(ivec3(50));
	cube.init();
	cube.download();
	/*unsigned int patchfactor = 2;
	vector<Sphere> patches;
	const int patchcount = 5;
	for(int i=0;i<patchcount;i++){
		for(int j=0;j<patchcount;j++){
			vec2 start = vec2(float(i),float(j));
			start /= float(patchcount);
			vec2 end = vec2(float(i+1),float(j+1));
			end /= float(patchcount);
			patches.emplace_back(Sphere(patchfactor++,start,end));
			patches.back().init();
			patches.back().download();
		}
	}*/

	bb = new Billboard(-0.5f,-0.5f,1.f,1.f);
	bb->init();
	bb->download();
	checkGlError("geometry");
	Ref<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	/*vs->compile("attribute vec4 in_Position;\n"
"attribute vec4 in_texCoords;\n"
"attribute vec4 in_Normal;\n"
"varying vec4 texCoords;\n"
"varying vec4 normal;\n"
"varying vec4 eyevec;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 projMatrix;\n"
"uniform vec4 camera;\n"
"void main(void)\n"
"{\n"
"	texCoords = in_Position;\n"
"	normal = vec4(normalize(transpose(inverse(mat3(viewMatrix))) * in_Normal.xyz),1.0);\n"
"	vec4 position = viewMatrix * in_Position;\n"
"	eyevec = normalize(position);\n"
"	gl_Position = projMatrix * position; \n"
"}\n");*/
	if(!vs->compileFromFile("displace.vert"))
		DebugBreak();
	Ref<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	/*fs->compile("uniform sampler2D framedata;"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	vec4 sample = texture2D(framedata,texCoords.st);\n"
"	gl_FragColor = vec4(texCoords.s,texCoords.t,1.-texCoords.s,1.0);\n"
"}\n");*/
	if(!fs->compileFromFile("seamless.frag"))
		DebugBreak();
	Ref<Shader> shader = Shader::Allocate();
	Ref<Shader> shaderCopy(shader);
	shader->addAttrib("in_Position",0);
	shader->addAttrib("in_texCoords",1);
	shader->addAttrib("in_Normal",2);
	shader->attachStage(vs);
	shader->attachStage(fs);
	shader->link();
	shader->bind();
	checkGlError("shader");
	glUniform1i(shader->getUniformLocation("framedata"), 0);
	TextureManager* texMan = TextureManager::Instance();
	TexRef tex = texMan->texFromFile("Hello.png");
	checkGlError("texFromFile");
	glActiveTexture(GL_TEXTURE0);
	tex->bind();
	checkGlError("bindTexture");
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	float i = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		handleKeys(window);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mat4 projview = projectionMatrix* camera.toMat4();
		glUniformMatrix4fv(shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, value_ptr(camera.toMat4()));
		glUniformMatrix4fv(shader->getUniformLocation("projMatrix"), 1, GL_FALSE, value_ptr(projectionMatrix));
		vec3 pos = camera.pos();
		glUniform4fv(shader->getUniformLocation("camera"), 1, value_ptr(vec4(camera.pos(),1.0)));
		cube.draw();
		/*for(int index=0;index<patches.size();index++){
			patches[index].draw();
		}*/
		glUniform1f(shader->getUniformLocation("time"),i);
		i+=0.001f;
		//bb->draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}