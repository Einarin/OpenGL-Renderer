#include "shader.h"
#include "glincludes.h"
#include <iostream>
#include <fstream>

namespace gl{

ShaderStage::ShaderStage(int stage)
{
	id = glCreateShader(stage);
}
std::shared_ptr<ShaderStage> ShaderStage::Allocate(int stage)
{
	return std::shared_ptr<ShaderStage>(new ShaderStage(stage));
}
bool ShaderStage::compile(std::string source)
{
	const char* src = source.c_str();
	int len = source.size();
	glShaderSource(id,1,&src,&len);
	glCompileShader(id);
	int compiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	GLsizei length;
	glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
	GLchar* buff = new GLchar[length];
	glGetShaderInfoLog(id,length,&length,buff);
	//do something with error log
	std::cout << buff << std::endl;
	delete[] buff;
	if (compiled != GL_TRUE)
	{
		//shader failed to compile
		return false;
	} 
	return true;
}

bool ShaderStage::compileFromFile(std::string filename){
	std::ifstream file(filename);
	if(!file.is_open()){
		return false;
	}
	std::istreambuf_iterator<char> eos;
	std::string s(std::istreambuf_iterator<char>(file), eos);
	return compile(s);
}

int ShaderStage::getId(){
	return id;
}

Shader::Shader() : id(0){
	id = glCreateProgram();
}
std::shared_ptr<Shader> Shader::Allocate(){
	return std::shared_ptr<Shader>(new Shader());
}
Shader::~Shader(){
	glDeleteProgram(id);
}

void Shader::attachStage(std::shared_ptr<ShaderStage> stage){
	stages.push_back(stage);
}

void Shader::addAttrib(std::string name, int index){
	glBindAttribLocation(id,index, name.c_str());
}

int Shader::getUniformLocation(std::string name){
	return glGetUniformLocation(id, name.c_str());
}

bool Shader::link(){
	for(auto it = stages.begin(); it != stages.end(); it++){
		glAttachShader(id,(*it)->getId());
	}
	glLinkProgram(id);
	int linked;
		
	glGetProgramiv(id,GL_LINK_STATUS, &linked);
	GLsizei length;
	glGetProgramiv(id,GL_INFO_LOG_LENGTH,&length);
	GLchar* buff = new GLchar[length];
	glGetProgramInfoLog(id,length,&length,buff);
	//do something with error log
	std::cout << buff << std::endl;
	if(linked != GL_TRUE)
	{
		//shader failed to link
		delete[] buff;
		return false;
	}

	return true;
}

void Shader::bind(){
	glUseProgram(id);
}

int Shader::getId(){
	return id;
}

} // namespace gl