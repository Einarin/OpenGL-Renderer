#include "FrameBufferObject.h"
#include "glincludes.h"
#include <iostream>

namespace gl{

FramebufferObject::FramebufferObject(void):id(0)
{
}
void FramebufferObject::BindDisplayBuffer(unsigned int target){
	glBindFramebuffer(target,0);
}
void FramebufferObject::init(){
	glGenFramebuffers(1, &id);
}
void FramebufferObject::drawBuffers(int count){
	bind(GL_DRAW_FRAMEBUFFER);
	if(count > MaxFboColorAttachments()){
		count = MaxFboColorAttachments();
		std::cout << "Clamping color attachments to implementation limit!" << std::endl;
	}
	GLenum* bufs = new GLenum[count];
	for(int i=0;i<count;i++){
		bufs[i] = GL_COLOR_ATTACHMENT0+i;
	}
	glDrawBuffers(count,bufs);
	delete[] bufs;
}
bool FramebufferObject::isComplete(unsigned int target){
	GLenum result = glCheckFramebufferStatus(target);
	if(result == GL_FRAMEBUFFER_COMPLETE){
		return true;
	}
	if(result == GL_FRAMEBUFFER_UNDEFINED){
		std::cout << "No framebuffer is bound!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT){
		std::cout << "Not all attachments are attachment complete!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
		std::cout << "No image attached to FBO!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER){
		std::cout << "Not all color attachments are defined!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE){
		std::cout << "Attachments have mismatched multisample!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS){
		std::cout << "Not all attachments are layered attachments!" << std::endl;
	}
	if(result == GL_FRAMEBUFFER_UNSUPPORTED){
		std::cout << "Framebuffer configuration unsupported by this implementation!" << std::endl;
	}
	return false;
}
void FramebufferObject::bind(unsigned int target){
	glBindFramebuffer(target,id);
}

void FramebufferObject::attachTexture(unsigned int target, unsigned int attachment, Tex2D tex, unsigned int level){
	bind(target);
	glFramebufferTexture(target,attachment,
		tex->getId(),
		level);
}
/*void FramebufferObject::attachTexture1D(unsigned int target, unsigned int attachment, Tex1D tex, unsigned int level){
	bind(target);
	glFramebufferTexture1D(target,attachment,
		tex->getType(),
		tex->getId(),
		level);
}*/
void FramebufferObject::attachTexture2D(unsigned int target, unsigned int attachment, Tex2D tex, unsigned int level){
	bind(target);
	glFramebufferTexture2D(target,attachment,
		tex->getType(),
		tex->getId(),
		level);
}
/*void FramebufferObject::attachTextureLayer(unsigned int target, unsigned int attachment, Tex3D tex, unsigned int level, unsigned int layer){
	bind(target);
	glFramebufferTextureLayer(target,attachment,
		static_cast<GlTexture*>(&*tex)->getId(),
		level,layer);
}*/
void FramebufferObject::attachRenderbuffer(unsigned int target, unsigned int attachment, RenderbufferObject& buffer){
	bind(target);
	glFramebufferRenderbuffer(target,attachment,GL_RENDERBUFFER,buffer.getId());
}
void FramebufferObject::attachDepthStencilRenderbuffer(unsigned int format){
	depthStencilRenderbuffer.init(format,Size);
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,depthStencilRenderbuffer.getId());
}
void FramebufferObject::attachDepthRenderbuffer(unsigned int format){
	depthRenderbuffer.init(format,Size);
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER,depthRenderbuffer.getId());
}
void FramebufferObject::attachStencilRenderbuffer(unsigned int format){
	stencilRenderbuffer.init(format,Size);
	bind(GL_DRAW_FRAMEBUFFER);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,stencilRenderbuffer.getId());
}

FramebufferObject::~FramebufferObject(void)
{
	if(id){
		glDeleteFramebuffers(1,&id);
	}
}

} //namespace gl
