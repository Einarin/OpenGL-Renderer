#include "StarRenderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
namespace gl{

StarRenderer::StarRenderer():ready(false)
{}


StarRenderer::~StarRenderer()
{}

bool StarRenderer::load(){
	bool success = true;
	geometry.generate(20,vec3(57));
	geometry.init();
	geometry.download();

	shader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compileFromFile("star.vert");
	success &= fs->compileFromFile("star.frag");
	shader->addAttrib("in_Position",0);
	shader->addAttrib("in_Normal",1);
	shader->addAttrib("in_Tangent",2);
	shader->addAttrib("in_TexCoords",3);
	shader->attachStage(vs);
	shader->attachStage(fs);
	success &= shader->link();
	shader->bind();
	checkGlError("StarRenderer shader");
	glUniform1i(shader->getUniformLocation("framedata"), 0);
	ready = success;
	return success;
}
void StarRenderer::draw(Camera* camera){
	shader->bind();
	glUniformMatrix4fv(shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->getUniformLocation("projMatrix"), 1, GL_FALSE, value_ptr(camera->GetProjectionMatrix()));
	checkGlError("set mats");
	glUniform4fv(shader->getUniformLocation("camera"), 1, value_ptr(vec4(camera->GetPosition(),1.0)));
	float time = glfwGetTime();
	glUniform1f(shader->getUniformLocation("time"),time*0.1);
	glUniform1i(shader->getUniformLocation("levels"), 2);
	checkGlError("setup draw");
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	geometry.draw();
}

} //namespace gl