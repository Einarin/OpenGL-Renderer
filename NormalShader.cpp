#include "NormalShader.h"
#include "glincludes.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

namespace gl{
NormalShader::NormalShader(void)
{
}


NormalShader::~NormalShader(void)
{
}

bool NormalShader::init(){
	bool success = true;
	auto nvs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	success &= nvs->compileFromFile("mvpnorm.vert");
	auto ngs = ShaderStage::Allocate(GL_GEOMETRY_SHADER);
	success &= ngs->compileFromFile("normals.geom");
	auto cfs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= cfs->compileFromFile("color.frag");
	m_shader = Shader::Allocate();
	m_shader->addAttrib("in_Position",0);
	m_shader->addAttrib("in_Normal",1);
	m_shader->attachStage(nvs);
	m_shader->attachStage(ngs);
	m_shader->attachStage(cfs);
	success &= m_shader->link();
	m_shader->bind();
	glUniform4fv(m_shader->getUniformLocation("color"), 1, value_ptr(vec4(0.0,0.0,1.0,1.0)));
	glUniform1f(m_shader->getUniformLocation("normalLength"), 0.05f);
	return success;
}

} //namespace gl
