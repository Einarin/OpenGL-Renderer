#include "GBuffer.h"
#include <iostream>

namespace gl {
	bool GBuffer::init()
	{
		m_gBuffer.init();
		return false;
	}

	bool GBuffer::setup(glm::ivec2 size)
	{
		m_gBuffer.Size = size;
		Albedo = Texture2D::backedTexture(GL_RGBA, GL_RGBA8, size, GL_UNSIGNED_BYTE);
		Normal = Texture2D::backedTexture(GL_RGBA, GL_RGBA16F, size, GL_FLOAT);
		Depth = Texture2D::backedTexture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, size, GL_FLOAT);
		Position = Texture2D::backedTexture(GL_RGBA, GL_RGBA16F, size, GL_FLOAT);
		m_gBuffer.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Albedo);
		m_gBuffer.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, Normal);
		m_gBuffer.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Position);
		m_gBuffer.attachTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Depth);
		//check that all is well
		m_gBuffer.bind(GL_DRAW_FRAMEBUFFER);
		if (!m_gBuffer.isComplete(GL_DRAW_FRAMEBUFFER)) {
			std::cout << "GBuffer FBO not complete!\n";
		}
		FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);
		return true;
	}
	void GBuffer::bind()
	{
		m_gBuffer.bind(GL_DRAW_FRAMEBUFFER);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, drawBuffers);
	}
}