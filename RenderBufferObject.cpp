#include "RenderBufferObject.h"
#include "glincludes.h"

namespace gl{

RenderbufferObject::RenderbufferObject(void):id(0)
{
}

void RenderbufferObject::init(unsigned int format, glm::ivec2 size){
	//it is recommended by the OpenGL wiki to just delete and make
	// a new renderbuffer if changing configuration to avoid framebuffer
	// completeness problems, so that's what we do here
	if(id){
		glDeleteRenderbuffers(1,&id);
	}
	glGenRenderbuffers(1,&id);
	glBindRenderbuffer(GL_RENDERBUFFER,id);
	glRenderbufferStorage(GL_RENDERBUFFER,format,size.x,size.y);
}

RenderbufferObject::~RenderbufferObject(void)
{
	if(id){
		glDeleteRenderbuffers(1,&id);
	}
}

} //namespace gl
