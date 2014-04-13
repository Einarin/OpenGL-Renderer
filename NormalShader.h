#pragma once
#include "c:\users\johnny\game\include\shader.h"

namespace gl{
class NormalShader
{
protected:
	std::shared_ptr<Shader> m_shader;
public:
	NormalShader(void);
	~NormalShader(void);
	bool init();
	std::shared_ptr<Shader> shader(){
		return m_shader;
	}
};

}
