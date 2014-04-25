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
#include "TransformFeedback.h"
#include "VertexAttribBuilder.h"
#include "StarRenderer.h"
#include "AsteroidRenderer.h"
#include "DynamicPatchSphere.h"
#include "CoreShaders.h"
#include "FrameBufferObject.h"

#include "windows.h" //for debugbreak only!

using namespace std;
using namespace gl;
using namespace glm;

//global variables ftw
Billboard* bb;
Camera camera;
glm::mat4 projectionMatrix;
glm::mat4 orthoMatrix;
int levels = 5;
int tessFactor = 50;

int main(int argc, char* argv[])
{
	GLFWwindow* window;

	glfwSetErrorCallback(onGlfwError);

	if (!glfwInit())
		exit(EXIT_FAILURE);
	//window = glfwCreateWindow(1680, 1050, "Game", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(1280, 800, "Game", NULL/*glfwGetPrimaryMonitor()*/, NULL);
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
	SetupSupport();
	
	Assimp::DefaultLogger::create("assimp.log",Assimp::Logger::VERBOSE,aiDefaultLogStream_STDOUT);

	//set glfw callbacks
	//controls handled by main loop instead
	glfwSetKeyCallback(window, onKeyPressed);
	glfwSetWindowSizeCallback(window, onResizeWindow);
	glfwSetCursorPosCallback(window, onCursorMoved);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	orthoMatrix = glm::ortho(0.f,static_cast<float>(width),0.f,static_cast<float>(height),-1.f,1.f);

	//Camera setup
	camera = Camera(); // vec3(0.,2.5,3.0),vec3(0,2.0,0),vec3(0,1.0,0)
	camera.SetPosition(vec3(0.f,2.f,-10.f));
	//camera.SetPosition(vec3(0.0,0.0,0.0));
	//camera.SetTarget(vec3(-4.f,-4.f,-4.f));
	camera.SetTarget(vec3(0.0,0.0,0.0));
	camera.SetAspectRatio(static_cast<float>(width)/static_cast<float>(height));

	cout << "generating assets...\n";
	
	//Sphere cube(32,vec2(0.0,0.0),vec2(1.0));
	SkyBox skybox;
	skybox.init();
	skybox.download();
	skybox.setImageAsync("assets/Skybox/skybox");

	StarRenderer star;
	if(!star.load()) DebugBreak();

	Cube cube;
	cube.generate(2,vec3(0));
	cube.ModelMatrix = mat4();
	cube.init();
	cube.download();
	
	cout << "loading models...\n";
	Model* model = NULL;
	const unsigned int asteroidFactor=5;
	AsteroidRenderer aRenderer;
	if(!aRenderer.setup()) DebugBreak();
	CpuPool.async([&aRenderer](){
		for(int q=0;q<27;q++){
			glm::vec3 position(q%3-1,q/3%3-1,q/9-1);
			if(position == vec3(0.f,0.f,0.f))
				continue;
			mat4 modelMat = translate(rotate(mat4(),3.14159f*0.25f,glm::vec3(1.f,2.f,3.f)),position*10.f);
			auto tmp = &aRenderer;
			CpuPool.await<bool>(
				glQueue.async<Future<bool>>([=]()->Future<bool>{
					return tmp->addAsteroidAsync(modelMat,position);
			}));
		}
	});
	/*while(!result.complete()){
		glPool.processMainQueueUnit();
	}*/
	
	CpuPool.async([&](){
		model = new Model("assets/fighter.obj");
		auto ptr = model;
		glQueue.async([=](){
			ptr->init();
			ptr->download();
		});
	});//*/

	bb = new Billboard(-0.5f,-0.5f,1.f,1.f);
	bb->init();
	bb->download();
	checkGlError("geometry");
	
	PatchSphere ps;
	ps.tesselate(8);
	ps.init();
	ps.download();

	DynamicPatchSphere psr;
	psr.init(8);

	glPointSize(3.0f);

	Light light;
	light.position = vec3(2.0,2.0,2.0);

	cout << "compiling shaders...\n";

	LightingShader ls;
	ls.init();
	MvpShader mls = ls;
	NormalShader ns;
	ns.init();
	MvpShader mns = ns;
	TexturedShader ts;
	if(!ts.init())
		DebugBreak();

	
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

	FramebufferObject fbo;
	fbo.init();
	fbo.Size = ivec2(1280,800);
	//TexRef tex = TextureManager::Instance()->texFromFile("Hello.png");
	TexRef tex = TextureManager::Instance()->backedTex(GL_RGBA,fbo.Size,GL_UNSIGNED_BYTE);
	fbo.attachTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tex);
	fbo.attachDepthStencilRenderbuffer(GL_DEPTH24_STENCIL8);
	if(!fbo.isComplete(GL_DRAW_FRAMEBUFFER)){
		DebugBreak();
	}
	FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	double time = glfwGetTime();
	double oldtime = time;
	int fpsCount = 10;
	int counter = 0;
	bool drawNormals = false;
	bool changingNormals = false;

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
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_PERIOD)){
			levels++;
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_COMMA)){
			levels--;
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_N)){
			if(!changingNormals){
				drawNormals = !drawNormals;
			}
			changingNormals = true;
		} else {
			changingNormals = false;
		}

		checkGlError("start main loop");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//fbo.bind(GL_DRAW_FRAMEBUFFER);
		
		//draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGlError("clear screen");
		
		skybox.draw(&camera);
		checkGlError("draw skybox");
		glEnable(GL_DEPTH_TEST);

		vec4 lightpos = vec4(-1.0f,-5.0f,10.0f,1.0f);

		mls.bind();
		mls.setView(camera.GetViewMatrix());
		mls.setProjection(camera.GetProjectionMatrix());
		checkGlError("set view and projection matrices");
		ShaderRef s = ls;
		glUniform4fv(s->getUniformLocation("camera"), 1, value_ptr(vec4(camera.GetPosition(),1.0)));
		glUniform4fv(s->getUniformLocation("light"), 1, value_ptr(lightpos));
		glUniform1f(s->getUniformLocation("time"),(float)time*0.1f);
		glUniform1i(s->getUniformLocation("levels"), levels);
		checkGlError("setup model shader");

		aRenderer.draw(mls);
		//mls.setModel(mat4());
		//ps.draw();
		checkGlError("asteroid renderer");
		if(drawNormals){
			mns.bind();
			mns.setView(camera.GetViewMatrix());
			mns.setProjection(camera.GetProjectionMatrix());
			aRenderer.draw(mns);
			//mns.setModel(mat4());
			//ps.draw();
		}

		if(model && model->ready()){
			((ShaderRef)ts)->bind();
			glUniform4fv(((ShaderRef)ts)->getUniformLocation("light"), 1, value_ptr(lightpos));
			LitTexMvpShader dts = ts;
			dts.setView(camera.GetViewMatrix());
			dts.setProjection(camera.GetProjectionMatrix());
			checkGlError("create DiffuseTexMvpShader");
			dts.bind();
			model->draw(dts);
			/*mls.bind();
			mls.setModel(mat4());
			model->draw();*/
			if(drawNormals){
				mns.bind();
				mns.setModel(mat4());
				LitTexMvpShader dns = ns;
				model->draw(dns);
			}
		}

		star.modelMatrix = translate(mat4(),-(vec3)lightpos.xyz);
		star.draw(&camera);
		FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);
		glDisable(GL_DEPTH_TEST);
		tex->draw();

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		textRenderer->draw(orthoMatrix);
		checkGlError("draw hello world");
		counter++;
		time = glfwGetTime();
		if(counter >= fpsCount){
			counter = 0;
			fps->clearText();
			checkGlError("fps clear text");
			double newtime = time;
			stringstream ss, ss1, ss2, ss3;
			ss.precision(3);
			
			glm::vec3 position = camera.GetPosition();
			ss << " (" << position[0] << ", " << position[1] << ", " << position[2] << ") ";
			//ss << "   up    (" << cam[1][0] << ", " << cam[1][1] << ", " << cam[1][2] << ")\n";*/
			ss << round(double(fpsCount) / (newtime - oldtime));
			ss << " FPS";
			//ss << " levels=" << levels;
			oldtime = newtime;
			fps->addText(ss.str(),vec2(5,height-32),vec4(1.0f));
			checkGlError("fps add text");
			//process async work
			if(glQueue.processQueueUnit() || !(model && model->ready()))
				fps->addText("Loading...",vec2(glm::max(width/2-100,5),height/2),vec4(1.0f));
			checkGlError("fps add loading");
		}
		checkGlError("about to draw text");
		fps->draw(orthoMatrix);
		checkGlError("draw text");
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}