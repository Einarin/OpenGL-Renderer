#include "glincludes.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "callbacks.h"
#include <iostream>
#include <sstream>
#include "Texture.h"
#include "Geometry.h"
#include "Shader.h"
#include "Model.h"
#include "ThreadPool.h"
#include "Text.h"
#include "assimp/DefaultLogger.hpp"
#include "SkyBox.h"

#include "windows.h" //for debugbreak only!

using namespace std;
using namespace gl;
using namespace glm;

//global variables ftw
Billboard* bb;
Camera camera;
glm::mat4 projectionMatrix;
glm::mat4 orthoMatrix;

//global threadpool
ThreadPool glPool(4);

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
	window = glfwCreateWindow(1200, 800, "Game", NULL, NULL);
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

	Assimp::DefaultLogger::create("assimp.log",Assimp::Logger::VERBOSE,aiDefaultLogStream_STDOUT);

	//set glfw callbacks
	//controls handled by main loop instead
	glfwSetKeyCallback(window, onKeyPressed);
	glfwSetWindowSizeCallback(window, onResizeWindow);
	glfwSetCursorPosCallback(window, onCursorMoved);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_DEPTH_TEST);

	/*{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		projectionMatrix = glm::perspective(120.f,static_cast<float>(width)/static_cast<float>(height),0.001f,1000.f);
	}*/
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(60.f,static_cast<float>(width)/static_cast<float>(height),0.001f,10000.f);
	orthoMatrix = glm::ortho(0.f,static_cast<float>(width),0.f,static_cast<float>(height),-1.f,1.f);

	//Camera setup
	camera = Camera(); // vec3(0.,2.5,3.0),vec3(0,2.0,0),vec3(0,1.0,0)
	//camera.SetPosition(vec3(-2.f,-2.f,-2.f));
	camera.SetPosition(vec3(0.0,0.0,0.0));
	camera.SetTarget(vec3(1.0,1.0,1.0));
	camera.SetAspectRatio(static_cast<float>(width)/static_cast<float>(height));

	cout << "generating assets...\n";
	
	//Sphere cube(32,vec2(0.0,0.0),vec2(1.0));

	Cube cube;
	cube.generate(2,vec3(0));
	cube.ModelMatrix = mat4();
	cube.init();
	cube.download();

	cout << "loading models...\n";
	Model* model = NULL;
	/*Cube asteroids[27];
	for(int i=0;i<27;i++){
		glPool.async([i,&asteroids](){
			glm::vec3 position(i%3-1,i/3%3-1,i/9-1);
			position *= 2.0f;
			asteroids[i].generate(40,position);
			asteroids[i].ModelMatrix = glm::translate(mat4(),position);
			auto ptr = &asteroids[i];
			glPool.onMain([=](){
				ptr->init();
				ptr->download();
			});
		});
	}*/
	
	/*glPool.async([&](){
		auto ptr = new Model("assets/shuttle.3ds");
		auto local = &model;
		glPool.onMain([=](){
			ptr->init();
			ptr->download();
			*local = ptr;
		});
	});*/

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
			patches.back().ModelMatrix = mat4();
			patches.back().init();
			patches.back().download();
		}
	}*/

	bb = new Billboard(-0.5f,-0.5f,1.f,1.f);
	bb->init();
	bb->download();
	checkGlError("geometry");

	Light light;
	light.position = vec3(2.0,2.0,2.0);

	cout << "compiling shaders...\n";

	std::shared_ptr<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
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
	std::shared_ptr<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	/*fs->compile("uniform sampler2D framedata;"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	vec4 sample = texture2D(framedata,texCoords.st);\n"
"	gl_FragColor = vec4(texCoords.s,texCoords.t,1.-texCoords.s,1.0);\n"
"}\n");*/
	if(!fs->compileFromFile("seamless.frag"))
		DebugBreak();
	std::shared_ptr<Shader> shader = Shader::Allocate();
	std::shared_ptr<Shader> shaderCopy(shader);
	shader->addAttrib("in_Position",0);
	shader->addAttrib("in_Normal",1);
	shader->addAttrib("in_Tangent",2);
	shader->addAttrib("in_TexCoords",3);
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
	float la[4];
	la[0] = light.position.x;
	la[1] = light.position.y;
	la[2] = light.position.z;
	la[3] = 1.f;
	glUniform4fv(shader->getUniformLocation("light"),1,la);

	SkyBox skybox;
	skybox.init();
	skybox.download();
	skybox.setImage("assets/Skybox/skybox");

	//grab the mouse last so we can do things during load
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	TextManager textMan;
	textMan.init();
	TextRenderer* textRenderer = textMan.getTextRenderer("DejaVuSans.ttf",24);
	textRenderer->loadAscii();
	TextRenderer* fps = textMan.getTextRenderer("DejaVuSans.ttf",32);
	fps->loadAscii();
	vec2 end = textRenderer->addText("Hello World!",vec2(5,5),vec4(1.0,1.0,1.0,1.0));

	checkGlError("setup text");

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	float i = 5.f;
	double time = glfwGetTime();
	int fpsCount = 60;
	int counter = 0;
	while (!glfwWindowShouldClose(window))
	{
		//input handling
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

		//process async work
		//todo: process this while we have time before next draw call
		glPool.processMainQueueUnit();
		checkGlError("start main loop");
		//draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGlError("clear screen");
		glDisable(GL_DEPTH_TEST);
		skybox.draw(&camera);
		glEnable(GL_DEPTH_TEST);

		//mat4 projview = projectionMatrix* camera.toMat4();
		shader->bind();
		glUniformMatrix4fv(shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(shader->getUniformLocation("projMatrix"), 1, GL_FALSE, value_ptr(camera.GetProjectionMatrix()));
		vec3 pos = camera.GetPosition();
		glUniform4fv(shader->getUniformLocation("camera"), 1, value_ptr(vec4(camera.GetPosition(),1.0)));
		glUniform4fv(shader->getUniformLocation("light"), 1, value_ptr(vec4(1.5f,3.0f,3.0f,1.0f)));
		/*for(int i=0;i<27;i++){
			glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(asteroids[i].ModelMatrix));
			asteroids[i].draw();
			asteroids[i].ModelMatrix = asteroids[i].ModelMatrix * glm::rotate(mat4(),0.01f*i,vec3(0,1,0));
		}*/
		glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(cube.ModelMatrix));
		checkGlError("setup model shader");
		//cube.draw();
		if(model)
			model->draw();
		/*for(int index=0;index<patches.size();index++){
			patches[index].draw();
		}*/
		glUniform1f(shader->getUniformLocation("time"),i);
		i+=0.00001f;
		//bb->draw();
		textRenderer->draw(orthoMatrix);
		counter++;
		if(counter >= fpsCount){
			counter = 0;
			fps->clearText();
			double newtime = glfwGetTime();
			stringstream ss, ss1, ss2, ss3;
			ss.precision(3);
			
			glm::vec3 position = camera.GetPosition();
			ss << " (" << position[0] << ", " << position[1] << ", " << position[2] << ") ";
			//ss << "   up    (" << cam[1][0] << ", " << cam[1][1] << ", " << cam[1][2] << ")\n";*/
			ss << double(fpsCount) / (newtime - time);
			ss << " FPS";
			time = newtime;
			fps->addText(ss.str(),vec2(5,height-32),vec4(1.0f));
			//vec3 pos = camera.pos();
			/*ss1 << " (" << cam[1][0] << ", " << cam[1][1] << ", " << cam[1][2] << ", " << cam[1][3] << ")";
			fps->addText(ss1.str(),vec2(5,height-64),vec4(1.0f));
			ss2 << " (" << cam[2][0] << ", " << cam[2][1] << ", " << cam[2][2] << ", " << cam[2][3] << ")";
			fps->addText(ss2.str(),vec2(5,height-(32*3)),vec4(1.0f));
			ss3 << " (" << cam[3][0] << ", " << cam[3][1] << ", " << cam[3][2] << ", " << cam[3][3] << ")";
			fps->addText(ss3.str(),vec2(5,height-(32*4)),vec4(1.0f));*/
			//if(!model)
				//fps->addText("Loading...",vec2(max(width/2-100,5),height/2),vec4(1.0f));
		}
		fps->draw(orthoMatrix);
		checkGlError("draw text");
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}