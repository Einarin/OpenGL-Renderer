#include "glincludes.h"
#include "glutCallbacks.h"
#include <iostream>
#include "Texture.h"
#include "Geometry.h"
#include "Shader.h"

using namespace std;
using namespace gl;

int main(int argc, char* argv[])
{
	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowSize(800,600);
	//glutCreateWindow("Game");
	GLFWwindow* window;
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

	glClearColor(0.0f,0.1f,0.3f,1.0f);
	//glutDisplayFunc(onRender);
	//glutReshapeFunc(onReshapeWindow);
	Billboard bb;
	bb.init();
	bb.download();
	checkGlError("geometry");
	GlShaderStage vs(GL_VERTEX_SHADER);
	vs.compile("attribute vec4 in_Position;\n"
"attribute vec4 in_texCoords;\n"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	texCoords = in_texCoords;\n"
"	gl_Position = in_Position; \n"
"}\n");
	GlShaderStage fs(GL_FRAGMENT_SHADER);
	fs.compile("uniform sampler2D framedata;"
"varying vec4 texCoords;\n"
"void main(void)\n"
"{\n"
"	vec4 sample = texture2D(framedata,texCoords.st);\n"
"	gl_FragColor = sample;\n"
"}\n");
	GlShader shader;
	int shaderId = shader.getId();
	glBindAttribLocation(shaderId,0, "in_Position");
	glBindAttribLocation(shaderId,1, "in_texCoords");
	shader.attachStage(vs);
	shader.attachStage(fs);
	shader.link();
	shader.bind();
	checkGlError("shader");
	glUniform1i(glGetUniformLocation(shaderId, "framedata"), 0);
	TextureManager* texMan = TextureManager::Instance();
	TexRef tex = texMan->texFromFile("Hello.png");
	checkGlError("texFromFile");
	glActiveTexture(GL_TEXTURE0);
	tex->bind();
	checkGlError("bindTexture");
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		bb.draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glutMainLoop();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}