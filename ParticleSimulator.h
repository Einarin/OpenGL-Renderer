#pragma once
#include "VertexArrayObject.h"
#include "TransformFeedback.h"
#include "Camera.h"

namespace gl {

class ParticleSimulator
{
protected:
	std::vector<TransformFeedback> m_feedbackBuffers;
	int m_activeBuffer;
	int m_particleCount;
	ShaderRef m_drawShader;
	ShaderRef m_processShader;
	TexRef m_bbtex;
public:
	ParticleSimulator();
	void setup(int particleCount);
	void update();
	void draw(Camera& c);
};

}
