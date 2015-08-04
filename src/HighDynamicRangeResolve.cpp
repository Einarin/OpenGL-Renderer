#include "HighDynamicRangeResolve.h"
#include "AssetManager.h"
#ifdef _DEBUG
#include <iostream>
#endif

namespace gl {

	bool HighDynamicRangeResolve::init(){
		m_fbo.init();
		m_bloomFbo.init();
		m_bb.init();
		m_bb.download();
		m_shader = Shader::Allocate();
		auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
		auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
		bool success = true;
		success &= vs->compile("#version 330\n"
			"in vec2 position;\n"
			"out vec2 texCoord;\n"
			"void main(void){\n"
			"texCoord = 0.5*(position+1.0);\n"
			"gl_Position = vec4(position,0.0,1.0);\n"
			"}\n");
		success &= fs->compileFromFile("glsl/hdr.frag");
		m_shader->attachStage(vs);
		m_shader->attachStage(fs);
		success &= m_shader->link();
		m_shader->bind();
		glUniform1i(m_shader->getUniformLocation("tex"), 0);
		glUniform1i(m_shader->getUniformLocation("bloomTex"), 1);

		m_bloomShader = Shader::Allocate();
		auto bfs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
		success &= bfs->compileFromFile("glsl/bloom.frag");
		m_bloomShader->attachStage(vs);
		m_bloomShader->attachStage(bfs);
		success &= m_bloomShader->link();
		m_bloomShader->bind();
		glUniform1i(m_bloomShader->getUniformLocation("tex"), 0);

		m_initialized = success;
		return success;
	}

	void HighDynamicRangeResolve::setup(glm::ivec2 size){
		m_fbo.Size = size;
		//m_bloomTex = TextureManager::Instance()->backedTex(GL_RGB, size / 2, GL_FLOAT, GL_RGB16F);
		m_bloomTex = Texture2D::backedTexture(GL_RGB, GL_RGB16F, size / 2, GL_FLOAT);
		//m_fpTex = TextureManager::Instance()->backedTex(GL_RGBA, m_fbo.Size, GL_FLOAT, GL_RGBA16F);
		m_fpTex = Texture2D::backedTexture(GL_RGBA, GL_RGBA16F, m_fbo.Size, GL_FLOAT);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_fbo.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fpTex);
		m_fbo.attachDepthRenderbuffer(GL_DEPTH_COMPONENT);
#ifdef _DEBUG
		m_fbo.bind(GL_DRAW_FRAMEBUFFER);
		if (!m_fbo.isComplete(GL_DRAW_FRAMEBUFFER)) {
			std::cout << "HDR Resolve FBO not complete!\n";
		}
#endif
		m_bloomFbo.Size = size/2;
		m_bloomFbo.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_bloomTex);
		m_bloomFbo.attachDepthRenderbuffer(GL_DEPTH_COMPONENT);
	}

	void HighDynamicRangeResolve::bind(){
		m_fbo.bind(GL_DRAW_FRAMEBUFFER);
	}

	void HighDynamicRangeResolve::draw(){
		//m_bloomFbo.bind(GL_DRAW_FRAMEBUFFER);
		//glViewport(0, 0, m_bloomFbo.Size.x, m_bloomFbo.Size.y);
		glActiveTexture(GL_TEXTURE0);
		static auto tex = AssetManager::Instance()->loadTextureFromFile("Hello", false);
		//m_fpTex->bind();
		tex->bind();
		//m_bloomShader->bind();
		//m_bb.draw();
		FramebufferObject::BindDisplayBuffer(GL_DRAW_FRAMEBUFFER);
		glViewport(0, 0, m_fbo.Size.x, m_fbo.Size.y);
		glActiveTexture(GL_TEXTURE0);
		tex->bind();
		glActiveTexture(GL_TEXTURE1);
		m_bloomTex->bind();
		m_shader->bind();
		m_bb.draw();
	}
}