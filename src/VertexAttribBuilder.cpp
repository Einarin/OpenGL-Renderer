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
		a.type = FLOAT_ATTRIB;
		typesize = sizeof(GLfloat);
		break;
	case INT_ATTRIB:
		a.type = INT_ATTRIB;
		typesize = sizeof(GLint);
		break;
	default:
		__debugbreak();
	}
	a.size = typesize*count;
	_attribs.push_back(a);
	if(!_manualSize){
		_size += a.size;
	}
	return *this;
}

VertexAttribBuilder& VertexAttribBuilder::pad(int bytes){
	Attrib a;
	a.type = PADDING_ATTRIB;
	a.size = bytes;
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

void VertexAttribBuilder::build() const{
	int offset = 0;
	int ind = 0;
	for(int i=0; i<(int)_attribs.size();i++){
		if (_attribs[i].type != PADDING_ATTRIB){
			if (_attribs[i].type == FLOAT_ATTRIB) {
				glVertexAttribPointer(ind,_attribs[i].count,GL_FLOAT,false,_size,(const GLvoid*)offset);
			}
			if (_attribs[i].type == INT_ATTRIB) {
				glVertexAttribIPointer(ind, _attribs[i].count, GL_INT, _size, (const GLvoid*)offset);
			}
			glEnableVertexAttribArray(ind);
			ind++;
		}
		offset += _attribs[i].size;
	}
}

VertexAttribBuilder::~VertexAttribBuilder(void)
{
}

}