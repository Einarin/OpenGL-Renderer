#include "VertexArrayObject.h"
#include "glincludes.h"

namespace gl{

VertexArrayObject::VertexArrayObject(){
	glGenVertexArrays(1, &vao);
}

VertexArrayObject::~VertexArrayObject(){
	if (ownedBuffers.size() > 0){
		glDeleteBuffers(ownedBuffers.size(), &ownedBuffers[0]);
	}
	glDeleteVertexArrays(0,&vao);
}

void VertexArrayObject::bind(){
	glBindVertexArray(vao);
}

unsigned int VertexArrayObject::addAndBindBuffer(int target){
	unsigned int buff;
	glGenBuffers(1,&buff);
	glBindBuffer(target,buff);
	ownedBuffers.push_back(buff);
	return buff;
}

} //namespace gl
