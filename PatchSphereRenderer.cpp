#include "PatchSphereRenderer.h"
#include "glincludes.h"
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

using namespace glm;

namespace gl{
PatchSphereRenderer::PatchSphereRenderer(void)
{
	for(int i=0;i<6;i++){
		facePatchCount[i] = 0;
	}
}


PatchSphereRenderer::~PatchSphereRenderer(void)
{
	for(int i=0;i<6;i++){
		if(facePatchCount[i] > 0)
			delete[] facePatches[i];
	}
}
std::function<vec3(vec3)> transform[] = {
	[](vec3 in)->vec3{
		return glm::vec3(in.xy,1.f);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,in.y,-1.f);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-1.f,in.x,-in.y);
	},
	[](vec3 in)->vec3{
		return glm::vec3(1.f,in.xy);
	},
	[](vec3 in)->vec3{
		return glm::vec3(in.x,1.f,-in.y);
	},
	[](vec3 in)->vec3{
		return glm::vec3(in.x,-1.f,in.y);
	}
};

std::function<vec3(vec3)> transform2[] = {
	[](vec3 in)->vec3{
		return in;
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,1.f-in.y,in.z);
	},
	[](vec3 in)->vec3{
		return glm::vec3(1.f-in.x,-in.y,in.z);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,-in.y,in.z);
	}
};
void PatchSphereRenderer::init(int baseFactor){
	for(int i=0;i<12;i++){
		m_edgeTessFactors[i] = baseFactor;
	}
	for(int i=0;i<6;i++){
		facePatches[i] = new Patch[4];
		facePatchCount[i] = 4;
		for(int j=0;j<4;j++){
			int tessFactor[] = {baseFactor,baseFactor,baseFactor,baseFactor};
			facePatches[i][j].tesselate(tessFactor,[=](vec3 in)->vec3{
				return normalize(transform[i](transform2[j](in)));
			});
			facePatches[i][j].init();
			facePatches[i][j].download();
		}
	}
	bool success = true;
	m_shader = Shader::Allocate();
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	success &= vs->compileFromFile("asteroid.vert");
	success &= fs->compileFromFile("asteroid.frag");
	m_shader->addAttrib("in_Position",0);
	m_shader->addAttrib("in_Normal",1);
	m_shader->addAttrib("in_Tangent",2);
	m_shader->addAttrib("in_TexCoords",3);
	m_shader->attachStage(vs);
	m_shader->attachStage(fs);
	success &= m_shader->link();
	m_shader->bind();
	checkGlError("drawShader");
	glUniform1i(m_shader->getUniformLocation("framedata"), 0);
}

void PatchSphereRenderer::draw(Camera* c){
	m_shader->bind();
	glUniformMatrix4fv(m_shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(c->GetViewMatrix()));
	glUniformMatrix4fv(m_shader->getUniformLocation("projMatrix"), 1, GL_FALSE, glm::value_ptr(c->GetProjectionMatrix()));
	glUniform4fv(m_shader->getUniformLocation("camera"), 1, glm::value_ptr(glm::vec4(c->GetPosition(),1.0)));
	glUniform4fv(m_shader->getUniformLocation("light"), 1, glm::value_ptr(glm::vec4(1.5f,3.0f,3.0f,1.0f)));
	float time = (float)glfwGetTime();
	glUniform1f(m_shader->getUniformLocation("time"),time*0.1f);
	glUniformMatrix4fv(m_shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(mat4()));
	for(int i=0;i<6;i++){
		for(int j=0;j<facePatchCount[i];j++){
			facePatches[i][j].draw();
		}
	}
}

}