#include "ParticleSimulator.h"
#include <glm/gtc/matrix_transform.hpp>

namespace gl {

	ParticleSimulator::ParticleSimulator() :m_feedbackBuffers(2, TransformFeedback(GL_POINTS)), m_activeBuffer(0)
{ }

void ParticleSimulator::setup(int particleCount){
	VertexAttribBuilder b;
	//position
	b.attrib(FLOAT_ATTRIB, 3);
	//velocity
	b.attrib(FLOAT_ATTRIB, 3);
	//color
	b.attrib(FLOAT_ATTRIB, 4);
	unsigned int bufferSize = particleCount*b.getAutoSize();
	for (int i = 0; i < m_feedbackBuffers.size();i++){
		m_feedbackBuffers[i].init();
		m_feedbackBuffers[i].setupVao(1, b);
		//glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffers[i].getBuffer());
		//glBufferData(GL_ARRAY_BUFFER, particleCount*b.getAutoSize(), nullptr, GL_STREAM_DRAW);
		m_feedbackBuffers[i].allocateStorage(bufferSize);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffers[0].getBuffer());
	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	unsigned int i = 0;
	for (int x = 0; x < 100; x++){
		for (int y = 0; y < 100; y++){
			for (int z = 0; z < 100; z++){
				//position
				ptr[i++] = 2.f* (float(x) / 100) - 1.f;
				ptr[i++] = 2.f* (float(y) / 100) - 1.f;
				ptr[i++] = 2.f* (float(z) / 100) - 1.f;
				//velocity
				ptr[i++] = 0.f;
				ptr[i++] = 0.f;
				ptr[i++] = 0.f;
				//color
				ptr[i++] = 0.f;
				ptr[i++] = 0.f;
				ptr[i++] = 1.f;
				ptr[i++] = 0.1f; //Alpha
			}
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	bool success = true;
	m_drawShader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compileFromFile("glsl/drawParticle.vert");
	success &= fs->compileFromFile("glsl/drawParticle.frag");
	m_drawShader->attachStage(vs);
	m_drawShader->attachStage(fs);
	m_drawShader->addAttrib("in_Position", 0);
	m_drawShader->addAttrib("in_Velocity", 1);
	m_drawShader->addAttrib("in_Color", 2);
	success &= m_drawShader->link();
	m_processShader = Shader::Allocate();
	auto vfs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	success &= vfs->compileFromFile("glsl/particleFeedback.vert");
	m_processShader->attachStage(vfs);
	m_processShader->addAttrib("in_Position", 0);
	m_processShader->addAttrib("in_Velocity", 1);
	m_processShader->addAttrib("in_Color", 2);
	const char* feedbackOutput[] = { "ex_Position", "ex_Velocity", "ex_Color" };
	m_processShader->setInterleavedOutput(feedbackOutput, 3);
	m_processShader->link();
}

void ParticleSimulator::update(){
	int next = (m_activeBuffer + 1) % m_feedbackBuffers.size();
	m_processShader->bind();
	m_feedbackBuffers[next].enable();
	m_feedbackBuffers[m_activeBuffer].draw(1000000);
	m_feedbackBuffers[next].disable();
	m_activeBuffer = next;
}

void ParticleSimulator::draw(Camera& c){
	MvpShader m_drawMvp(m_drawShader);
	m_drawMvp.bind();
	m_drawMvp.setProjection(c.GetProjectionMatrix());
	m_drawMvp.setView(c.GetViewMatrix());
	m_drawMvp.setModel(glm::scale(glm::mat4(),glm::vec3(50.f,50.f,50.f)));
	m_feedbackBuffers[m_activeBuffer].draw(1000000);
}

} //namespace gl