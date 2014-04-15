#include "AsteroidRenderer.h"
#include "Geometry.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

namespace gl{

AsteroidRenderer::AsteroidRenderer(void):m_setup(false)
{
}


AsteroidRenderer::~AsteroidRenderer(void)
{
}

bool AsteroidRenderer::setup(){
	bool success = true;
	feedbackShader = Shader::Allocate();
	auto feedbackvs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	success &= feedbackvs->compileFromFile("feedbackdisplace.vert");
	feedbackShader->addAttrib("in_Position",0);
	feedbackShader->addAttrib("in_Normal",1);
	feedbackShader->addAttrib("in_Tangent",2);
	feedbackShader->addAttrib("in_TexCoords",3);
	feedbackShader->attachStage(feedbackvs);
	const char* feedbackOutput[] = { "position","normal","tangent","texCoords"};
	feedbackShader->setInterleavedOutput(feedbackOutput, 4);
	checkGlError("set feedback varyings");
	feedbackShader->link();
	feedbackShader->bind();
	checkGlError("feedbackShader");

	m_setup = success;
	return success;
}

const unsigned int tessfactor = 32;

//In order to avoid hanging while generating asteroids
//	this function behaves as asynchronously as possible
Future<bool> AsteroidRenderer::addAsteroidAsync(glm::mat4 modelMatrix, glm::vec3 seed){
	Asteroid asteroid;
	asteroid.modelMatrix = modelMatrix;
	m_asteroids.emplace_back(asteroid);
	int index = m_asteroids.size()-1;
	Future<bool> result;
	if(!m_setup){
		DebugBreak();
		result.set(false);
		return result;
	}
	CpuPool.async([=](){
		//variables to pass to inner lambdas
		auto objptr = this;
		int i = index;
		auto retval = result;
		auto s1 = seed;
		Asteroid* ast = &m_asteroids[i];
		auto theShader = feedbackShader;

		PatchSphere* pAsteroid = new PatchSphere();
		//pAsteroid->generate(tessfactor,s1,false);
		pAsteroid->tesselate(tessfactor);
		glQueue.async([=](){
			//variables to pass to inner lambda
			auto pAst = pAsteroid;
			int ind = i;
			auto s2 = s1;
			auto status = retval;
			auto shader = theShader;		
			auto a = ast;

			pAsteroid->init();
			pAsteroid->download();
			a->tfGeometry.init();
			glQueue.async([=](){
				shader->bind();
				a->tfGeometry.allocateStorage(tessfactor*tessfactor*6*6*4*sizeof(GLfloat)*12);
				VertexAttribBuilder b;
				b.attrib(FLOAT_ATTRIB,3);
				b.attrib(FLOAT_ATTRIB,3);
				b.attrib(FLOAT_ATTRIB,3);
				b.attrib(FLOAT_ATTRIB,3);
				a->tfGeometry.setupVao(4,b);
				a->tfGeometry.enable();
				glUniformMatrix4fv(shader->getUniformLocation("transformMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4()));
				glUniform3fv(shader->getUniformLocation("seed"),1,glm::value_ptr(s2));
				glUniform1i(shader->getUniformLocation("levels"), 12);
				pAst->draw();
				a->tfGeometry.disable();
				delete pAst;
				auto result = status;
				result.set(true);
				a->generated = true;
			});
		});
	});
	return result;
}

void AsteroidRenderer::draw(MvpShader s){
	for(int i=0; i < m_asteroids.size();i++){
		if(m_asteroids[i].generated){
			s.setModel(m_asteroids[i].modelMatrix);
			m_asteroids[i].tfGeometry.draw();
		}
	}
}

} // namespace gl