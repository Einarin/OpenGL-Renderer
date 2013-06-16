#include "shader.h"
#include "glincludes.h"
#include <iostream>

namespace gl{

GlShaderStage::GlShaderStage(int stage)
{
	id = glCreateShader(stage);
}
GlShaderStage::GlShaderStage(const GlShaderStage& other)
{
	id = other.id;
}
bool GlShaderStage::compile(std::string source)
{
	const char* src = source.c_str();
	int len = source.size();
	glShaderSource(id,1,&src,&len);
	glCompileShader(id);
	int compiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE)
	{
		//shader failed to link
		GLsizei length;
		glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
		GLchar* buff = new GLchar[length];
		glGetShaderInfoLog(id,length,&length,buff);
		//do something with error log
		std::cout << buff << std::endl;
		delete[] buff;
		return false;
	} 
	return true;
}

int GlShaderStage::getId(){
	return id;
}

GlShader::GlShader() : id(0){
	id = glCreateProgram();
}

GlShader::~GlShader(){
	glDeleteProgram(id);
}

void GlShader::attachStage(GlShaderStage& stage){
	stages.push_back(stage);
}

bool GlShader::link(){
	for(unsigned int i=0;i<stages.size();i++){
		glAttachShader(id,stages.at(i).getId());
	}
	glLinkProgram(id);
	int linked;
		
	glGetProgramiv(id,GL_LINK_STATUS, &linked);
	if(linked != GL_TRUE)
	{
		//shader failed to link
		GLsizei length;
		glGetProgramiv(id,GL_INFO_LOG_LENGTH,&length);
		GLchar* buff = new GLchar[length];
		glGetProgramInfoLog(id,length,&length,buff);
		//do something with error log
		std::cout << buff << std::endl;
		delete[] buff;
		return false;
	}

	return true;
}

void GlShader::bind(){
	glUseProgram(id);
}

int GlShader::getId(){
	return id;
}

} // namespace gl