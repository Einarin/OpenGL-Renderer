#pragma once
#include "glincludes.h"
#include "Texture2.h"
#include "Shader.h"
#include "Geometry.h"
#include "FrameBufferObject.h"

namespace gl {

class FrameProcessor {
protected:
	gl::Tex2D m_tex;
	gl::ShaderRef m_shader;
	gl::Billboard m_bb;
public:
	void init(ShaderStageRef fs, glm::ivec2 size, GLint format, GLint internalFormat, GLenum datatype);
	setTex(Gl::Tex2D inputTex);
	run();
};

}