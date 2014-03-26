#pragma once
#include "glincludes.h"
#include "renderer.h"
#include "Geometry.h"

namespace gl {
class StarRenderer :
	public Renderer
{
protected:
	Cube geometry;
	std::shared_ptr<Shader> shader;
	bool ready;
public:
	glm::mat4 modelMatrix;
	StarRenderer();
	~StarRenderer();
	bool load();
	virtual void draw(Camera* c);
};

}