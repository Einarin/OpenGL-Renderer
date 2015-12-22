#include "FrameProcessor.h"

void gl::FrameProcessor::init(ShaderStageRef fs, glm::ivec2 size, GLint format, GLint internalFormat, GLenum datatype)
{
	m_tex = Texture2D::backedTexture(format, internalFormat, size, datatype);

}
