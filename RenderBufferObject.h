#include "glm/glm.hpp"
#include <memory>
#pragma once

namespace gl{

class RenderbufferObject
{
protected:
	unsigned int id;
public:
	RenderbufferObject();
	void init(unsigned int format, glm::ivec2 size);
	unsigned int getId(){
		return id;
	}
	~RenderbufferObject(void);
};

}
