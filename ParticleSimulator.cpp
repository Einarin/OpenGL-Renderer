#include "ParticleSimulator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

namespace gl {

	ParticleSimulator::ParticleSimulator() :m_feedbackBuffers(2, TransformFeedback(GL_POINTS)), m_activeBuffer(0), m_particleCount(0)
{ 
	m_bbtex = TextureManager::Instance()->texFromFile("assets/Comet_Rosetta.png");
}

void ParticleSimulator::setup(int particleCount){
	m_bbtex->init();
	VertexAttribBuilder b;
	//position
	b.attrib(FLOAT_ATTRIB, 3);
	//velocity
	b.attrib(FLOAT_ATTRIB, 3);
	//color
	b.attrib(FLOAT_ATTRIB, 4);
	unsigned int bufferSize = particleCount*particleCount*particleCount*b.getAutoSize();
	for (int i = 0; i < m_feedbackBuffers.size();i++){
		m_feedbackBuffers[i].init();
		m_feedbackBuffers[i].setupVao(1, b);
		//glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffers[i].getBuffer());
		//glBufferData(GL_ARRAY_BUFFER, particleCount*b.getAutoSize(), nullptr, GL_STREAM_DRAW);
		m_feedbackBuffers[i].allocateStorage(bufferSize);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_feedbackBuffers[0].getBuffer());
	std::mt19937 mtgen;
	std::uniform_real_distribution<float> dist(-1000.f, 1000.f);
	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	unsigned int i = 0;
	for (int x = 0; x < particleCount; x++){
		for (int y = 0; y < particleCount; y++){
			for (int z = 0; z < particleCount; z++){
				//position
				ptr[i++] = 2.f* (float(x) / particleCount) - 1.f;
				ptr[i++] = 2.f* (float(y) / particleCount) - 1.f;
				ptr[i++] = 2.f* (float(z) / particleCount) - 1.f;
				//ptr[i++] = 0.f; ptr[i++] = 0.f; ptr[i++] = 0.f;
				//velocity
				ptr[i++] = dist(mtgen);
				ptr[i++] = dist(mtgen);
				ptr[i++] = dist(mtgen);
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
	auto gs = ShaderStage::Allocate(GL_GEOMETRY_SHADER);
	success &= vs->compileFromFile("glsl/drawParticle.vert");
	success &= fs->compileFromFile("glsl/drawParticle.frag");
	success &= gs->compileFromFile("glsl/billboard.geom");
	m_drawShader->attachStage(vs);
	m_drawShader->attachStage(fs);
	m_drawShader->attachStage(gs);
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
	if (success){
		m_drawShader->bind();
		glUniform1i(m_drawShader->getUniformLocation("tex"), 0);
		m_particleCount = particleCount*particleCount*particleCount;
	}
}

void ParticleSimulator::update(){
	int next = (m_activeBuffer + 1) % m_feedbackBuffers.size();
	m_processShader->bind();
	m_feedbackBuffers[next].enable();
	m_feedbackBuffers[m_activeBuffer].draw(m_particleCount);
	m_feedbackBuffers[next].disable();
	m_activeBuffer = next;
}

void ParticleSimulator::draw(Camera& c){
	MvpShader m_drawMvp(m_drawShader);
	m_drawMvp.bind();
	glActiveTexture(GL_TEXTURE0);
	m_bbtex->bind();
	m_drawMvp.setProjection(c.GetProjectionMatrix());
	m_drawMvp.setView(c.GetViewMatrix());
	m_drawMvp.setModel(glm::scale(glm::mat4(),glm::vec3(500.f,500.f,500.f)));
	glUniform3fv(m_drawShader->getUniformLocation("cameraPosition"),1, glm::value_ptr(c.GetPosition()));
	m_feedbackBuffers[m_activeBuffer].draw(m_particleCount);
}

} //namespace gl