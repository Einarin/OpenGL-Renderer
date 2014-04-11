#pragma once
#include "renderer.h"
#include "Geometry.h"

namespace gl {
class PatchSphereRenderer :
	public Renderer
{
protected:
	int m_edgeTessFactors[12];
	Patch* facePatches[6];
	int facePatchCount[6];
	std::shared_ptr<Shader> m_shader;
public:
	PatchSphereRenderer(void);
	~PatchSphereRenderer(void);
	void init(int baseFactor);
	virtual void draw(Camera* c);
};

}
