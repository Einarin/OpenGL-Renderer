#include "HighDynamicRangeResolve.h"

namespace gl {

	bool HighDynamicRangeResolve::init(){
		m_fbo.init();
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
		m_initialized = success;
		return success;
	}

	void HighDynamicRangeResolve::setup(glm::ivec2 size){
		m_fbo.Size = size;
		m_fpTex = TextureManager::Instance()->backedTex(GL_RGBA, m_fbo.Size, GL_FLOAT, GL_RGBA16F);
		m_fbo.attachTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fpTex);
		m_fbo.attachDepthRenderbuffer(GL_DEPTH_COMPONENT);
	}

	void HighDynamicRangeResolve::bind(){
		m_fbo.bind(GL_DRAW_FRAMEBUFFER);
	}

	void HighDynamicRangeResolve::draw(){
		m_shader->bind();
		glActiveTexture(GL_TEXTURE0);
		m_fpTex->bind();
		m_bb.draw();
	}
}