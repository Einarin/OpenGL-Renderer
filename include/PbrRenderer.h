#pragma once
#include "glincludes.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

namespace gl {

	struct PbrObject {
		glm::mat4 modelMatrix;
		/*GLuint colorMetalness;
		GLuint normalRoughness;*/
		glm::vec3 color;
		float metalness;
		float roughness;
		GLuint vao;
		GLenum elementType;
		unsigned int elementCount;
		unsigned int elementOffset;
	};

	class PbrRenderer {
	protected:
		gl::ShaderRef m_shader;
		GLint m_projectionMatrixIndex;
		GLint m_viewMatrixIndex;
		GLint m_modelMatrixIndex;
		GLint m_cameraIndex;
		GLint m_lightIndex;
		GLint m_lightColorIndex;
		GLint m_materialColorIndex;
		GLint m_metalnessIndex;
		GLint m_roughnessIndex;
	public:
		bool init() {
			bool status = true;
			auto pvs = ShaderStage::Allocate(GL_VERTEX_SHADER);
			auto pfs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
			status &= pvs->compileFromFile("glsl/mvpNormals.vert");
			status &= pfs->compileFromFile("glsl/lighting.frag");
			m_shader = Shader::Allocate();
			m_shader->attachStage(pvs);
			m_shader->attachStage(pfs);
			m_shader->addAttrib("in_Position", 0);
			m_shader->addAttrib("in_Normal", 1);
			m_shader->addAttrib("in_TexCoord", 2);
			status &= m_shader->link();
			m_shader->bind();
			GLuint id = m_shader->getId();
			m_projectionMatrixIndex = glGetUniformLocation(id, "projectionMatrix");
			m_viewMatrixIndex = glGetUniformLocation(id, "viewMatrix");
			m_modelMatrixIndex = glGetUniformLocation(id, "modelMatrix");
			m_cameraIndex = glGetUniformLocation(id, "cameraWorldPosition");
			m_lightIndex = glGetUniformLocation(id, "lightPosition");
			m_lightColorIndex = glGetUniformLocation(id, "lightColor");
			m_materialColorIndex = glGetUniformLocation(id, "materialColor");
			m_metalnessIndex = glGetUniformLocation(id, "metalness");
			m_roughnessIndex = glGetUniformLocation(id, "roughness");
			return status;
		}
		void draw(Camera& camera,const Light& light, PbrObject* objects, int numObjects) {
			m_shader->bind();
			glUniformMatrix4fv(m_projectionMatrixIndex, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
			glUniformMatrix4fv(m_viewMatrixIndex, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
			glUniform3fv(m_cameraIndex, 1, glm::value_ptr(camera.GetPosition()));
			glUniform3f(m_lightIndex, light.position.x, light.position.y, light.position.z);
			glUniform3f(m_lightColorIndex, 10.f, 10.f, 10.f);
			for (int i = 0; i < numObjects; i++) {
				glUniform3f(m_materialColorIndex, 1.0f, 0.766f, 0.336f);//gold specular color
				glUniform3fv(m_materialColorIndex, 1, glm::value_ptr(objects[i].color));
				glUniform1f(m_metalnessIndex, objects[i].metalness);//metal
				glUniform1f(m_roughnessIndex, objects[i].roughness);//rough
				glUniformMatrix4fv(m_modelMatrixIndex, 1, false, glm::value_ptr(objects[i].modelMatrix));
				glBindVertexArray(objects[i].vao);
				glDrawElements(GL_TRIANGLES, objects[i].elementCount, objects[i].elementType, (void*)objects[i].elementOffset);
			}
		}
	};

} //namespace gl
