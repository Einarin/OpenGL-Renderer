#pragma once
#include "shader.h"

namespace gl{

class CoreShader{
protected:
	ShaderRef m_shader;
public:
	virtual bool init()=0;
operator ShaderRef(){
	return m_shader;
}
operator MvpShader(){
	return MvpShader(m_shader);
}
operator LitTexMvpShader(){
	return LitTexMvpShader(m_shader);
}
};

class NormalShader : public CoreShader
{
public:
	bool init();
};

class LightingShader : public CoreShader
{
public:
	bool init();
};

class TexturedShader : public CoreShader
{
	public:
	bool init();
};

} //namespace gl
