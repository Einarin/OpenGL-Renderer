#pragma once
#include "FrameBufferObject.h"
#include "Shader.h"
#include "Geometry.h"

namespace gl {

	class HighDynamicRangeResolve {
	protected:
		bool m_initialized;
		FramebufferObject m_fbo;
		TexRef m_fpTex;
		ShaderRef m_shader;
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
