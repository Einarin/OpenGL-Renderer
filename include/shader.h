#pragma once
#include "Refcounted.h"
#include <string>
#include <list>

namespace gl
{
	class ShaderStage {
	protected:
		int id;
		ShaderStage(int stage);
	public:
		static Ref<ShaderStage> Allocate(int stage);
		bool compile(std::string source);
		bool compileFromFile(std::string filename);
		int getId();
	};
	
	class Shader {
	protected:
		std::list<Ref<ShaderStage>> stages;
		int id;
		Shader();
	public:
		~Shader();
		static Ref<Shader> Allocate();
		void attachStage(Ref<ShaderStage> stage);
		void addAttrib(std::string name, int index);
		int getUniformLocation(std::string name);
		bool link();
		void bind();
		int getId();
	};

}//namespace gl