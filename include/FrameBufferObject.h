#pragma once
#include "glm/glm.hpp"
#include "Texture2.h"
#include "RenderBufferObject.h"

namespace gl {

class FramebufferObject
{
protected:
	unsigned int id;
	//these are only used if requested
	RenderbufferObject depthRenderbuffer;
	RenderbufferObject stencilRenderbuffer;
	RenderbufferObject depthStencilRenderbuffer;
	void attachRenderbuffer(unsigned int target, unsigned int attachment, RenderbufferObject& buffer);
public:
	glm::ivec2 Size;
	FramebufferObject(void);
	void init();
	/// this helper makes MRT easy for the common case, set up hard cases yourself :P
	void drawBuffers(int count);
	///pass GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER, or just GL_FRAMEBUFFER
	///		for target
	bool isComplete(unsigned int target);
	static void BindDisplayBuffer(unsigned int target);
	void bind(unsigned int target);
	/// unless you're doing fancy things you can just use this
	void attachTexture(unsigned int target, unsigned int attachment, Tex2D tex, unsigned int level=0);
	//void attachTexture1D(unsigned int target, unsigned int attachment, Tex2D tex, unsigned int level=0);
	void attachTexture2D(unsigned int target, unsigned int attachment, Tex2D tex, unsigned int level=0);
	/// This is used for 3D textures
	//void attachTextureLayer(unsigned int target, unsigned int attachment, Tex3D tex, unsigned int level=0, unsigned int layer=0);
	//For convenience the Renderbuffers are owned by the FBO for now
	void attachDepthStencilRenderbuffer(unsigned int format);
	void attachDepthRenderbuffer(unsigned int format);
	void attachStencilRenderbuffer(unsigned int format);
	~FramebufferObject(void);
};

}