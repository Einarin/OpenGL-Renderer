#pragma once
#include "Shader.h"
#include "Property.h"

namespace gl{

class CoreShader{
protected:
	ShaderRef m_shader;
public:
	virtual bool init()=0;
	inline void bind(){
		m_shader->bind();
	}
	RoProp<MvpShader> mvpShader;
	RoProp<LitTexMvpShader> litTexMvpShader;
	operator ShaderRef(){
		return m_shader;
	}
	operator MvpShader(){
		return MvpShader(m_shader);
	}
	operator LitTexMvpShader(){
		return LitTexMvpShader(m_shader);
	}
	CoreShader():mvpShader([=]()->MvpShader{return MvpShader(m_shader);}),
		litTexMvpShader([=]()->LitTexMvpShader{return LitTexMvpShader(m_shader);})
	{}
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

class ColorPosShader : public CoreShader
{
public:
	bool init();
};

} //namespace gl
