#pragma once
#include "Refcounted.h"
#include <string>
#include <list>
#include <memory>
#include <glm/glm.hpp>

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

}//namespace gl