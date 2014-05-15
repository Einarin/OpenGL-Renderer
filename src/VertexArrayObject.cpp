#include "VertexArrayObject.h"
#include "glincludes.h"

namespace gl{

VertexArrayObject::VertexArrayObject(){
	glGenVertexArrays(1, &vao);
}

VertexArrayObject::~VertexArrayObject(){
	glDeleteVertexArrays(ownedBuffers.size(),&ownedBuffers[0]);
}

void VertexArrayObject::bind(){
	glBindVertexArray(vao);
}

void VertexArrayObject::addAndBindBuffer(int target){
	unsigned int buff;
	glGenBuffers(1,&buff);
	glBindBuffer(target,buff);
	ownedBuffers.push_back(buff);
}

} //namespace gl
