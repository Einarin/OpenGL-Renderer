#pragma once
#include "FrameBufferObject.h"

namespace gl {
	class GBuffer {
	protected:
		FramebufferObject m_gBuffer;
	public:
		Tex2D Albedo;
		Tex2D Normal;
		Tex2D Depth;
		Tex2D Position;
		bool init();
		bool setup(glm::ivec2 size);
		void bind();
	};
	
}