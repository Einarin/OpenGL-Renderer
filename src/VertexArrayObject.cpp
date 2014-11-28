#include "VertexArrayObject.h"
#include "glincludes.h"

namespace gl{

VertexArrayObject::VertexArrayObject(){
	glGenVertexArrays(1, &vao);
}

VertexArrayObject::~VertexArrayObject(){
	if (ownedBuffers.size() > 0){
		glDeleteBuffers(ownedBuffers.size(), &ownedBuffers[0].second);
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
	ownedBuffers.push_back(std::make_pair(target,buff));
	return buff;
}

void VertexArrayObject::bindBuffer(int index){
	glBindBuffer(ownedBuffers[index].first, ownedBuffers[index].second);
}

} //namespace gl
