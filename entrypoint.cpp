#include "glincludes.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
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
#include "AssetManager.h"
#include "HighDynamicRangeResolve.h"
#include "ParticleSimulator.h"

using namespace std;
using namespace gl;
using namespace glm;

//global variables ftw
Billboard* bb;
Camera camera;
glm::mat4 projectionMatrix;
glm::mat4 orthoMatrix;
HighDynamicRangeResolve hdr;
int levels = 5;
int tessFactor = 50;
double fpsTarget = 60.0;
extern bool fullscreen;
extern bool cursorGrabbed;

int main(int argc, char* argv[])
{
	GLFWwindow* window;

	glfwSetErrorCallback(onGlfwError);

	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_RESIZABLE,GL_TRUE);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE);
#endif
	//glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	if(fullscreen){
		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		int count;
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		window = glfwCreateWindow(mode->width, mode->height, "Game", primary, NULL);
	} else {
		window = glfwCreateWindow(1280, 800, "Game", NULL/*glfwGetPrimaryMonitor()*/, NULL);
	}
	if (!window)
	{
		DebugBreak();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "glewInit failed, aborting." << endl;
		DebugBreak();
		glfwTerminate();
		return -1;
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	if(!SetupSupport()){
		cout << "Setup failed! Bailing out..." << endl;
		DebugBreak();
		glfwTerminate();
		return -1;
	}
	
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

	//setup the text renderer so we can draw Loading...
	TextManager textMan;
	textMan.init();
	TextRenderer* fps = textMan.getTextRenderer("DejaVuSans.ttf",32);
	fps->loadAscii();
	//draw a frame right away so we don't look frozen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND); //text is alpha blended
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	fps->addText("Loading...",vec2(glm::max(width/2-100,5),height/2),vec4(1.0f));
	fps->draw(orthoMatrix);
	glfwSwapBuffers(window);

	// BEGIN LOADING!

	//Camera setup
	camera = Camera(); // vec3(0.,2.5,3.0),vec3(0,2.0,0),vec3(0,1.0,0)
	camera.SetPosition(vec3(0.f,2.f,-10.f));
	//camera.SetPosition(vec3(0.0,0.0,0.0));
	//camera.SetTarget(vec3(-4.f,-4.f,-4.f));
	camera.SetNearDistance(0.01f);
	camera.SetViewDistance(20000.f);
	camera.SetTarget(vec3(0.0,0.0,0.0));
	camera.SetAspectRatio(static_cast<float>(width)/static_cast<float>(height));
		
	hdr.init();
	hdr.setup(glm::ivec2(width, height));

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
	AssetManager assetManager;
	const unsigned int asteroidFactor=5;
	AsteroidRenderer aRenderer;
	if(!aRenderer.setup()) DebugBreak();
	Future<bool> asteroidsGenerated;
    /*CpuPool.async([&aRenderer,asteroidsGenerated]() mutable{
		std::mt19937 mtgen;
		std::uniform_real_distribution<float> dist(1.f,2.f);
        for(int q=0;q<50;q++){
			glm::vec3 position(dist(mtgen)-1.5f,dist(mtgen)-1.5f,dist(mtgen)-1.5f);
			if(length(position) < 0.1f) //we don't want asteroids at the origin
				continue;
			position *= 100.f;
			float uniformScale = dist(mtgen);
			mat4 modelMat = scale(mat4(),uniformScale*vec3(dist(mtgen),dist(mtgen),dist(mtgen)));
			modelMat = translate(rotate(modelMat,3.14159f*0.5f*dist(mtgen),glm::vec3(dist(mtgen),dist(mtgen),dist(mtgen))),position);
			auto tmp = &aRenderer;
			CpuPool.await<bool>(
				glQueue.async<Future<bool>>([=]()->Future<bool>{
					return tmp->addAsteroidAsync(modelMat,position);
			}));
		}
		asteroidsGenerated.set(true);
	});
	while(!asteroidsGenerated.isDone()){
		glQueue.processQueueUnit();
	}//*/
	//aRenderer.buildTree();
    auto astMade = aRenderer.addAsteroidAsync(translate(mat4(),vec3(0.f,1.f,0.f)),vec3(0.f));
	
	std::shared_ptr<Model> model = assetManager.loadModel("assets/missile.obj");
	std::shared_ptr<Model> model2 = assetManager.loadModel("assets/MakeHuman/woman.obj");

    /*CpuPool.async([&](){
		model = new Model("assets/missile.obj");
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
	light.position = vec3(2.0,-2.0,2.0);

	cout << "compiling shaders...\n";

	auto noiseShader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	vs->compile("#version 330\n"
					"in vec2 position;\n"
					"out vec2 texCoord;\n"
					"void main(void){\n"
					"texCoord = 0.5*(position+1.0);\n"
					"gl_Position = vec4(position,0.0,1.0);\n"
					"}\n");
	fs->compileFromFile("glsl/crater.frag");
	noiseShader->attachStage(vs);
	noiseShader->attachStage(fs);
	noiseShader->link();
	Billboard noisebb;
	noisebb.init();
	noisebb.download();

	LightingShader ls;
	ls.init();
	MvpShader mls = ls;
	NormalShader ns;
	ns.init();
	MvpShader mns = ns;
	TexturedShader ts;
	if(!ts.init())
		DebugBreak();
	ColorPosShader boundingBoxShader;
	boundingBoxShader.init();
	MvpShader mvpbb(boundingBoxShader);
	TextRenderer* textRenderer = textMan.getTextRenderer("DejaVuSans.ttf",24);
	textRenderer->loadAscii();
	vec2 end = textRenderer->addText("Hello World!",vec2(5,5),vec4(1.0,1.0,1.0,1.0));

	checkGlError("setup text");

	FramebufferObject fbo;
	fbo.init();
	fbo.Size = ivec2(1280,800);
	//TexRef tex = TextureManager::Instance()->texFromFile("Hello.png");
	TexRef tex = TextureManager::Instance()->backedTex(GL_RGBA, fbo.Size, GL_FLOAT, GL_RGBA16F);
	TexRef depthTex = TextureManager::Instance()->backedTex(GL_DEPTH_COMPONENT,fbo.Size,GL_FLOAT);
	checkGlError("backedTex");
	fbo.attachTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tex);
	checkGlError("attachTexture");
	//fbo.attachDepthStencilRenderbuffer(GL_DEPTH24_STENCIL8);
	fbo.attachTexture(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,depthTex);
	checkGlError("attachDepthStencilRenderbuffer");
	if(!fbo.isComplete(GL_DRAW_FRAMEBUFFER)){
		DebugBreak();
	}
	FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	double time = glfwGetTime();
	double fpstime = time;
	int fpsCount = 10;
	int lastruns = 0;
	int counter = 0;
	float loadingVal = 2.f;
	bool drawNormals = false;
	bool changingNormals = false;

	//finish loading before we jump into the main loop
	if(glQueue.processQueueUnit() || !asteroidsGenerated.isDone())
	{
		glfwPollEvents();
	}
	//grab the mouse last so we can do things during load
	/*if(!cursorGrabbed){
		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
		cursorGrabbed = true;
	}*/

	ParticleSimulator particles;
	particles.setup(100);

	while (!glfwWindowShouldClose(window))
	{
		//know when we got to the top of the render loop
		double frameStart = glfwGetTime();

		//kick off gpu transform feedback calculations
		particles.update();

		//input handling
		glfwPollEvents();
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
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_P)){
			aRenderer.reset();
			asteroidsGenerated=true;
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
		
		//bind fullscreen HDR buffer
		//fbo.bind(GL_DRAW_FRAMEBUFFER);
		hdr.bind();

		//draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGlError("clear screen");
		if (wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		skybox.draw(&camera);
		checkGlError("draw skybox");
		glEnable(GL_DEPTH_TEST);
		
		mls.bind();
		mls.setView(camera.GetViewMatrix());
		mls.setProjection(camera.GetProjectionMatrix());
		checkGlError("set view and projection matrices");
		ShaderRef s = ls;
		glUniform4fv(s->getUniformLocation("camera"), 1, value_ptr(vec4(camera.GetPosition(),1.0)));
		glUniform4fv(s->getUniformLocation("light"), 1, value_ptr(light.position));
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
		mvpbb.bind();
		mvpbb.setView(camera.GetViewMatrix());
		mvpbb.setProjection(camera.GetProjectionMatrix());
		//aRenderer.drawBoundingBoxes(mvpbb);

		if(model.use_count() > 0 && model->ready()){
			model->ModelMatrix = rotate(translate(mat4(),vec3(-5.f,-3.f, 6.f)),-20.f,vec3(0.f,1.f,0.f));
			ts.bind();
			glUniform4fv(((ShaderRef)ts)->getUniformLocation("light"), 1, value_ptr(light.position));
			LitTexMvpShader dts = ts.litTexMvpShader;
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
				LitTexMvpShader dns = ns;
				model->draw(dns);
			}
		}
		//model->drawBoundingBoxes(&camera);
		if(model2.use_count() > 0 && model2->ready()){
			model2->ModelMatrix = translate(rotate(mat4(),210.f,vec3(0.f,1.f,0.f)),vec3(-5.f,-13.f,-6.f));
			ts.bind();
			glUniform4fv(((ShaderRef)ts)->getUniformLocation("light"), 1, value_ptr(light.position));
			LitTexMvpShader dts = ts.litTexMvpShader;
			dts.setView(camera.GetViewMatrix());
			dts.setProjection(camera.GetProjectionMatrix());
			checkGlError("create DiffuseTexMvpShader");
			dts.bind();
			model2->draw(dts);
			/*mls.bind();
			mls.setModel(mat4());
			model->draw();*/
			if(drawNormals){
				mns.bind();
				LitTexMvpShader dns = ns;
				model2->draw(dns);
			}
		}
		//model2->drawBoundingBoxes(&camera);
		star.modelMatrix = translate(mat4(),-vec3(light.position));
		
		particles.draw(camera);
		star.draw(&camera);

		//End scene drawing
		FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable(GL_BLEND);
		if (wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		hdr.draw();
		//tex->draw();
		//depthTex->draw();
		//noiseShader->bind();
		//noisebb.draw();
		
		textRenderer->draw(orthoMatrix);
		checkGlError("draw hello world");
		counter++;
		
		//if(counter >= fpsCount){
			counter = 0;
			fps->clearText();
			checkGlError("fps clear text");
			stringstream ss, ss1, ss2, ss3;
			ss.precision(3);

			int runs=0;
			//process async work
			if(glQueue.processQueueUnit() || model.use_count() > 0 && !( model->ready())){
				runs++;
				loadingVal = 2.f;
			}
			if(loadingVal > 0.f){
				fps->addText("Loading...",vec2(glm::max(width/2-100,5),height/2),vec4(1.0f,1.f,1.f,loadingVal));
				checkGlError("fps add loading");
				loadingVal -= 0.01f;
			}
			//let's see if we have time left over
			time = glfwGetTime()-frameStart;
			double targetTime =  1.0/fpsTarget;
			while(time < targetTime){
				if(glQueue.processQueueUnit()){
					runs++;
				} else {
					//do we have enough time to sleep?
					int dt = static_cast<int>(500.0 *(targetTime-time));
					if(dt > 0){
						//We have time left over to meet our frame target
						//	and no queued work to do
						//do we have GPU memory?
						int freeMem = FreeGpuMemoryMB();
						//Obviously we need MOAR ASTEROIDS!
						if(false && freeMem > 100000 && asteroidsGenerated.isDone() && asteroidsGenerated){
							const int count = 5;
							cout << "Generating " << count << " asteroids with " << freeMem << " bytes available" << endl;
							CpuPool.async([&aRenderer,asteroidsGenerated,count,time]() mutable{
								static std::mt19937 mtgen(1000.0*time);
								static std::uniform_real_distribution<float> dist(1.f,2.f);
								Future<bool> futs[5];
								for(int q=0;q<count;q++){
									glm::vec3 position(dist(mtgen)-1.5f,dist(mtgen)-1.5f,dist(mtgen)-1.5f);
									if(length(position) < 0.1f) //we don't want asteroids at the origin
										continue;
									position *= 100.f;
									float uniformScale = dist(mtgen);
									mat4 modelMat = scale(mat4(),uniformScale*vec3(dist(mtgen),dist(mtgen),dist(mtgen)));
									modelMat = translate(rotate(modelMat,3.14159f*0.5f*dist(mtgen),glm::vec3(dist(mtgen),dist(mtgen),dist(mtgen))),position);
									auto tmp = &aRenderer;
									//CpuPool.await<bool>(
									futs[q] = glQueue.async<Future<bool>>([=]()->Future<bool>{
											return tmp->addAsteroidAsync(modelMat,position);
									});//);
								}
								bool success = true;
								for(int q=0;q<count;q++){
									success &= CpuPool.await(futs[q]);
								}
								asteroidsGenerated.set(success);
							});
						} else {
							if(dt > 5){ //lets not oversleep!
								//cout << "s " << dt << " ";
#ifdef USE_STD_THREAD
								std::this_thread::sleep_for(std::chrono::milliseconds(dt));
#else
								Sleep(dt);
#endif
							}
						}
					}
					break;
				}
				time = glfwGetTime()-frameStart;
			}
			ss3 << aRenderer.asteroidCount() << " asteroids";
			fps->addText(ss3.str(),vec2(5,height-96),vec4(1.f));
			if(runs > 0 || loadingVal > 0.f){
				if(runs > 0) lastruns = runs;
				ss1 << "ran " << lastruns << " tasks";
				fps->addText(ss1.str(),vec2(5,height-128),vec4(1.f,1.f,1.f,loadingVal));
			}
			
			glm::vec3 position = camera.GetPosition();
			ss << "(" << position[0] << ", " << position[1] << ", " << position[2] << ") ";
			//ss << "   up    (" << cam[1][0] << ", " << cam[1][1] << ", " << cam[1][2] << ")\n";*/
			ss2 << round(1.0 / fpstime);
			ss2 << " FPS";
			//ss << " levels=" << levels;
			fps->addText(ss.str(),vec2(5,height-32),vec4(1.0f));
			fps->addText(ss2.str(),vec2(5,height-64),vec4(1.0f));
			checkGlError("fps add text");
		//}
		checkGlError("about to draw text");
		fps->draw(orthoMatrix);
		checkGlError("draw text");
		glfwSwapBuffers(window);
		fpstime = glfwGetTime()-frameStart;
	}
	//delete dynamic allocations
	delete fps;
	delete textRenderer;

	//close window and shutdown glfw
	//for some reason this is hideously slow sometimes (seems to be hanging on something...)
	/*double startClose = glfwGetTime();
	glfwDestroyWindow(window); 
	double windowClose = glfwGetTime() - startClose;
	glfwTerminate();*/
	return 0;
}
