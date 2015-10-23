#pragma once
#include "FrameBufferObject.h"
#include "Shader.h"
#include "Geometry.h"

namespace gl {

	class HighDynamicRangeResolve {
	protected:
		bool m_initialized;
		FramebufferObject m_fbo;
		FramebufferObject m_bloomFbo;
		Tex2D m_fpTex;
		Tex2D m_bloomTex;
		ShaderRef m_shader;
		ShaderRef m_bloomShader;
		Billboard m_bb;
	public:
		HighDynamicRangeResolve() : m_initialized(false)
		{}
		bool init();
		void setup(glm::ivec2 size);
		void bind();
		void draw();
	};

}
