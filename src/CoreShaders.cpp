#include "CoreShaders.h"
#include "glincludes.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

namespace gl{

bool NormalShader::init(){
	bool success = true;
	auto nvs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	//success &= nvs->compileFromFile("glsl/mvpnorm.vert");
	success &= nvs->compileFromFile("glsl/model.vert");
	auto ngs = ShaderStage::Allocate(GL_GEOMETRY_SHADER);
	//success &= ngs->compileFromFile("glsl/normals.geom");
	success &= ngs->compileFromFile("glsl/ssnormals.geom");
	auto cfs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= cfs->compileFromFile("glsl/attribColor.frag");
	m_shader = Shader::Allocate();
	m_shader->addAttrib("in_Position",0);
	m_shader->addAttrib("in_Normal",1);
	m_shader->addAttrib("in_Tangent",2);
	m_shader->addAttrib("in_TexCoords", 3);
	m_shader->attachStage(nvs);
	m_shader->attachStage(ngs);
	m_shader->attachStage(cfs);
	success &= m_shader->link();
	m_shader->bind();
	glUniform4fv(m_shader->getUniformLocation("color"), 1, value_ptr(vec4(0.0,0.0,1.0,1.0)));
	glUniform1f(m_shader->getUniformLocation("normalLength"), 0.05f);
	return success;
}

bool LightingShader::init(){
	bool success = true;
	m_shader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compileFromFile("glsl/asteroid.vert");
	success &= fs->compileFromFile("glsl/asteroid.frag");
	m_shader->addAttrib("in_Position",0);
	m_shader->addAttrib("in_Normal",1);
	m_shader->addAttrib("in_Tangent",2);
	m_shader->addAttrib("in_TexCoords",3);
	m_shader->attachStage(vs);
	m_shader->attachStage(fs);
	success &= m_shader->link();
	m_shader->bind();
	checkGlError("drawShader");
	glUniform1i(m_shader->getUniformLocation("framedata"), 0);
	return success;
}

bool TexturedShader::init(){
	bool success = true;
	m_shader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compileFromFile("glsl/model.vert");
	success &= fs->compileFromFile("glsl/model.frag");
	m_shader->addAttrib("in_Position",0);
	m_shader->addAttrib("in_Normal",1);
	m_shader->addAttrib("in_Tangent",2);
	m_shader->addAttrib("in_TexCoords",3);
	m_shader->attachStage(vs);
	m_shader->attachStage(fs);
	success &= m_shader->link();
	m_shader->bind();
	checkGlError("drawShader");
	//glUniform4fv(m_shader->getUniformLocation("color"), 1, value_ptr(vec4(0.0, 0.0, 1.0, 1.0)));
	//glUniform1f(m_shader->getUniformLocation("normalLength"), 0.05f);
	return success;
}

bool ColorPosShader::init(){
	bool success = true;
	m_shader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compile("#version 330\n"
				"uniform mat4 modelMatrix;\n"
				"uniform mat4 viewMatrix;\n"
				"uniform mat4 projMatrix;\n"
				"in vec3 position;\n"
				"void main(void){\n"
				"gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position,1.0);\n"
				"}\n");
	success &= fs->compileFromFile("glsl/color.frag");
	m_shader->addAttrib("position",0);
	m_shader->attachStage(vs);
	m_shader->attachStage(fs);
	success &= m_shader->link();
	m_shader->bind();
	float greenColor[] = {0.0f,1.0f,0.0f,1.0f};
	glUniform4fv(m_shader->getUniformLocation("color"),1,greenColor);
	return success;
}

} //namespace gl
