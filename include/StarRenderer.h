#pragma once
#include "glincludes.h"
#include "Renderer.h"
#include "Geometry.h"

namespace gl {
class StarRenderer :
	public Renderer
{
protected:
	PatchSphere geometry;
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
