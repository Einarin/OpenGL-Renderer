#pragma once
#include "Refcounted.h"
#include <string>
#include <vector>

namespace gl
{
	class GlShaderStage {
	protected:
		int id;
	public:
		GlShaderStage(int stage);
		GlShaderStage(const GlShaderStage& other);
		bool compile(std::string source);
		int getId();
		
	};
	
	class Shader : public Refcounted{
	public:
		virtual void bind()=0;
	};

	class GlShader : public Shader{
	protected:
		std::vector<GlShaderStage> stages;
		int id;
	public:
		void attachStage(GlShaderStage& stage);
		virtual bool link();
		virtual void bind();
		GlShader();
		~GlShader();
		int getId();
	};

	//typedef Ref<Shader> ShaderRef;

}//namespace gl