#pragma once
#include "Refcounted.h"
#include "glincludes.h"
#include <string>
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADER(SOURCE) #SOURCE

namespace gl
{
	class ShaderStage {
	protected:
		int id;
		ShaderStage(int stage);
	public:
		~ShaderStage();
		static std::shared_ptr<ShaderStage> Allocate(int stage);
		bool compile(std::string source);
		bool compileFromFile(std::string filename);
		int getId();
	};

	class Shader;
	typedef std::shared_ptr<Shader> ShaderRef;
		
	class Shader {
	protected:
		std::list<std::shared_ptr<ShaderStage>> stages;
		int id;
		Shader();
	public:
		~Shader();
		static ShaderRef Allocate();
		static ShaderRef Create(std::string vs, std::string fs);
		void attachStage(std::shared_ptr<ShaderStage> stage);
		void addAttrib(std::string name, int index);
		int getUniformLocation(std::string name);
		void setInterleavedOutput(const char** varyings, int count);
		bool link();
		void bind();
		int getId();
	};

	class MvpShader {
	protected:
		ShaderRef m_shader;
		int modelLoc;
		int viewLoc;
		int projLoc;
	public:
		MvpShader(ShaderRef s);
		void bind(){
			m_shader->bind();
		}
		void setModel(glm::mat4 modelMatrix);
		void setView(glm::mat4 viewMatrix);
		void setProjection(glm::mat4 projectionMatrix);
	};

	class LitTexMvpShader : public MvpShader {
	protected:
		int diffuseTex;
		int ambientLocation;
		int specularLocation;
		int shininessLocation;
	public:
		LitTexMvpShader(ShaderRef s);
		inline void setDiffuseTexActive(){
			glActiveTexture(GL_TEXTURE0);
		}
		inline void setAmbient(glm::vec3 ambient){
			glUniform3fv(ambientLocation,1,glm::value_ptr(ambient));
		}
		inline void setSpecular(glm::vec3 specular){
			glUniform3fv(specularLocation,1,glm::value_ptr(specular));
		}
		inline void setShininess(float shininess){
			glUniform1f(shininessLocation,shininess);
		}
	};

}//namespace gl
