#pragma once
#include "Refcounted.h"
#include <string>
#include <list>
#include <memory>

/*namespace glm {
	class mat4;
	class vec3;
}*/

namespace gl
{
	class ShaderStage {
	protected:
		int id;
		ShaderStage(int stage);
	public:
		static std::shared_ptr<ShaderStage> Allocate(int stage);
		bool compile(std::string source);
		bool compileFromFile(std::string filename);
		int getId();
	};
	
	class Shader {
	protected:
		std::list<std::shared_ptr<ShaderStage>> stages;
		int id;
		Shader();
	public:
		~Shader();
		static std::shared_ptr<Shader> Allocate();
		void attachStage(std::shared_ptr<ShaderStage> stage);
		void addAttrib(std::string name, int index);
		int getUniformLocation(std::string name);
		void setInterleavedOutput(const char** varyings, int count);
		bool link();
		void bind();
		int getId();
	};

}//namespace gl