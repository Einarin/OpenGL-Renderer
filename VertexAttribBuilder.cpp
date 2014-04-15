#include "VertexAttribBuilder.h"
#include "glincludes.h"

namespace gl{

VertexAttribBuilder::VertexAttribBuilder(void): _size(0),_manualSize(false)
{
}

VertexAttribBuilder& VertexAttribBuilder::attrib(AttribDataTypes type, int count)
{
	Attrib a;
	a.count = count;
	int typesize;
	switch(type){
	case FLOAT_ATTRIB:
		a.type = GL_FLOAT;
		typesize = 4;
	}
	a.size = typesize*count;
	_attribs.push_back(a);
	if(!_manualSize){
		_size += a.size;
	}
	return *this;
}

VertexAttribBuilder& VertexAttribBuilder::setSize(unsigned int size){
	_manualSize = true;
	_size = size;
	return *this;
}

void VertexAttribBuilder::build(){
	int offset = 0;
	for(int i=0; i<(int)_attribs.size();i++){
		glVertexAttribPointer(i,_attribs[i].count,_attribs[i].type,false,_size,(const GLvoid*)offset);
		glEnableVertexAttribArray(i);
		offset += _attribs[i].size;
	}
}

VertexAttribBuilder::~VertexAttribBuilder(void)
{
}

}