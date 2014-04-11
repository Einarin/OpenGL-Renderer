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
#include "PatchSphereRenderer.h"

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
	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowSize(800,600);
	//glutCreateWindow("Game");
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

	/*{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		projectionMatrix = glm::perspective(120.f,static_cast<float>(width)/static_cast<float>(height),0.001f,1000.f);
	}*/
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(60.f,static_cast<float>(width)/static_cast<float>(height),0.00001f,100.f);
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
			mat4 modelMat = translate(rotate(mat4(),3.14159f*0.25f,glm::vec3(1.f,2.f,3.f)),position*10.f);
			auto result = aRenderer.addAsteroidAsync(modelMat,position);
			CpuPool.await(result);
		}
	});
	/*while(!result.complete()){
		glPool.processMainQueueUnit();
	}*/
	
	CpuPool.async([&](){
		auto ptr = new Model("assets/fighter.obj");
		auto local = &model;
		glQueue.async([=](){
			ptr->init();
			ptr->download();
			*local = ptr;
		});
	});//*/

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

	/*Patch pc[6];
	//pc.genPatch2(4,4,8,8);
	//pc.genPatch2(2,16,4,8);
	//pc.genPatch(6,5,5,5);
	vec3 corners[][2] = {
		{vec3(-1.0f,-1.0f,1.0f),vec3(1.0f,1.0f,1.0f)},
		{vec3(1.0f,1.0f,-1.0f),vec3(-1.0f,-1.0f,-1.0f)},
		{vec3(-1.0f,-1.0f,-1.0f),vec3(-1.0f,1.0f,1.0f)},
		{vec3(1.0f,-1.0f,-1.0f),vec3(1.0f,1.0f,1.0f)},
		{vec3(-1.0f,-1.0f,-1.0f),vec3(1.0f,-1.0f,1.0f)},
		{vec3(-1.0f,1.0f,-1.0f),vec3(1.0f,1.0f,1.0f)}
	};
	int tess[][4] = {
		{32,8,32,8},
		{32,8,32,8},
		{8,16,8,16},
		{8,16,8,16},
		{32,16,32,16},
		{32,16,32,16}
	};
	std::function<vec3(vec3)> transform[] = {
		[](vec3 in)->vec3{
			return glm::vec3(2.f*in.xy-1.f,1.f);
		},
		[](vec3 in)->vec3{
			return glm::vec3(1.f-2.f*in.x,2.f*in.y-1.f,-1.f);
		},
		[](vec3 in)->vec3{
			return glm::vec3(-1.f,2.f*in.x-1.f,1.f-2.f*in.y);
		},
		[](vec3 in)->vec3{
			return glm::vec3(1.f,2.f*in.xy-1.f);
		},
		[](vec3 in)->vec3{
			return glm::vec3(2.f*in.x-1,1.f,1.f-2.f*in.y);
		},
		[](vec3 in)->vec3{
			return glm::vec3(2.f*in.x-1.f,-1.f,2.f*in.y-1.f);
		}
	};
	for(int i=0;i<6;i++){
		pc[i].tesselate(tess[i],transform[i]);
		pc[i].init();
		pc[i].download();
	}*/

	PatchSphere ps;
	ps.tesselate(8);
	ps.init();
	ps.download();

	PatchSphereRenderer psr;
	psr.init(8);

	glPointSize(3.0f);

	Light light;
	light.position = vec3(2.0,2.0,2.0);

	cout << "compiling shaders...\n";

	std::shared_ptr<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	if(!vs->compileFromFile("mvp.vert"))
		DebugBreak();
	std::shared_ptr<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	/*fs->compile("uniform sampler2D framedata;"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	vec4 sample = texture2D(framedata,texCoords.st);\n"
"	gl_FragColor = vec4(texCoords.s,texCoords.t,1.-texCoords.s,1.0);\n"
"}\n");*/
	if(!fs->compileFromFile("asteroid.frag"))
		DebugBreak();
	std::shared_ptr<Shader> shader = Shader::Allocate();
	shader->addAttrib("in_Position",0);
	shader->addAttrib("in_Normal",1);
	shader->addAttrib("in_Tangent",2);
	shader->addAttrib("in_TexCoords",3);
	shader->attachStage(vs);
	shader->attachStage(fs);
	if(!shader->link()){
		DebugBreak();
	}
	shader->bind();
	checkGlError("shader");
	glUniform1i(shader->getUniformLocation("framedata"), 0);

	auto nvs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	if(!nvs->compileFromFile("mvpnorm.vert"))
		DebugBreak();
	auto ngs = ShaderStage::Allocate(GL_GEOMETRY_SHADER);
	if(!ngs->compileFromFile("normals.geom"))
		DebugBreak();
	auto cfs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	if(!cfs->compileFromFile("color.frag"))
		DebugBreak();
	auto normShader = Shader::Allocate();
	normShader->addAttrib("in_Position",0);
	normShader->addAttrib("in_Normal",1);
	normShader->attachStage(nvs);
	normShader->attachStage(ngs);
	normShader->attachStage(cfs);
	if(!normShader->link()){
		DebugBreak();
	}
	normShader->bind();
	checkGlError("normShader");
		
	/*TextureManager* texMan = TextureManager::Instance();
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
	glUniform4fv(shader->getUniformLocation("light"),1,la);*/

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
	double time = glfwGetTime();
	double oldtime = time;
	int fpsCount = 10;
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
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_PERIOD)){
			levels++;
		}
		if(GLFW_PRESS == glfwGetKey(window,GLFW_KEY_COMMA)){
			levels--;
		}

		//process async work
		//todo: process this while we have time before next draw call
		//glPool.processMainQueueUnit();
		checkGlError("start main loop");
		//draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGlError("clear screen");
		
		skybox.draw(&camera);
						
		//mat4 projview = projectionMatrix* camera.toMat4();
		shader->bind();
		glUniformMatrix4fv(shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(shader->getUniformLocation("projMatrix"), 1, GL_FALSE, value_ptr(camera.GetProjectionMatrix()));
		vec3 pos = camera.GetPosition();
		glUniform4fv(shader->getUniformLocation("camera"), 1, value_ptr(vec4(camera.GetPosition(),1.0)));
		glUniform4fv(shader->getUniformLocation("light"), 1, value_ptr(vec4(-3.0f,-3.0f,-3.0f,1.0f)));
		glUniform1f(shader->getUniformLocation("time"),time*0.1);
		glUniform1i(shader->getUniformLocation("levels"), levels);
		checkGlError("setup model shader");

		aRenderer.draw(&camera);
		checkGlError("asteroid renderer");
		glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(mat4()));
		/*for(int i=0;i<6;i++){
			pc[i].draw();
		}*/
		//ps.draw();
		//psr.draw(&camera);

		//glBindVertexArray(vao);
		/*for(int i=0;i<asteroidCount;i++){
			glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(asteroids[i].ModelMatrix));
			glUniform3fv(shader->getUniformLocation("seed"),1,value_ptr(vec3(i%3-1,i/3%3-1,i/9-1)));
			//asteroids[0].draw();
			
			//glCullFace(GL_FRONT);
			//tf.draw();
			//glCullFace(GL_BACK);
			//asteroids[i].ModelMatrix =  glm::rotate(asteroids[i].ModelMatrix,0.1f,vec3(0,1,0));
			checkGlError("draw asteroid");
		}*/

		

		if(model){
			glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(mat4()));
			model->draw();
		}

		normShader->bind();
		checkGlError("bind normal shader");
		//glBindVertexArray(vao);
		glUniform4fv(normShader->getUniformLocation("color"), 1, value_ptr(vec4(0.0,0.0,1.0,1.0)));
		glUniform1f(normShader->getUniformLocation("normalLength"), 0.1f);
		glUniformMatrix4fv(normShader->getUniformLocation("viewMatrix"), 1, GL_FALSE, value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(normShader->getUniformLocation("projMatrix"), 1, GL_FALSE, value_ptr(camera.GetProjectionMatrix()));
			//glUniformMatrix4fv(normShader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(asteroids[0].ModelMatrix));
			//asteroids[0].draw();
			/*if(model){
				glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(mat4()));
				model->draw();
			}*/
		glUniformMatrix4fv(normShader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(mat4()));
		checkGlError("setup normal shader");
			//ps.draw();
			//tf.draw();
			//asteroids[i].ModelMatrix =  glm::rotate(asteroids[i].ModelMatrix,0.1f,vec3(0,1,0));
			checkGlError("draw asteroid normals");
		
		glBindVertexArray(0);
		
		
		//glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(cube.ModelMatrix));
		//cube.draw();
		
		/*for(int index=0;index<patches.size();index++){
			patches[index].draw();
		}*/
		//glUniform1f(shader->getUniformLocation("time"),time);
		//bb->draw();
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//star.draw(&camera);
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
			ss << " levels=" << levels;
			oldtime = newtime;
			fps->addText(ss.str(),vec2(5,height-32),vec4(1.0f));
			checkGlError("fps add text");
			//vec3 pos = camera.pos();
			/*ss1 << " (" << cam[1][0] << ", " << cam[1][1] << ", " << cam[1][2] << ", " << cam[1][3] << ")";
			fps->addText(ss1.str(),vec2(5,height-64),vec4(1.0f));
			ss2 << " (" << cam[2][0] << ", " << cam[2][1] << ", " << cam[2][2] << ", " << cam[2][3] << ")";
			fps->addText(ss2.str(),vec2(5,height-(32*3)),vec4(1.0f));
			ss3 << " (" << cam[3][0] << ", " << cam[3][1] << ", " << cam[3][2] << ", " << cam[3][3] << ")";
			fps->addText(ss3.str(),vec2(5,height-(32*4)),vec4(1.0f));*/
			if(glQueue.processQueueUnit())
				fps->addText("Loading...",vec2(max(width/2-100,5),height/2),vec4(1.0f));
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