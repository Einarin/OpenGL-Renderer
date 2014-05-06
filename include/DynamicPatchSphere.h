#pragma once
#include "Renderer.h"
#include "Geometry.h"

namespace gl {
class DynamicPatchSphere
{
protected:
	int m_edgeTessFactors[12];
	Patch* facePatches[6];
	int facePatchCount[6];
public:
	DynamicPatchSphere(void);
	~DynamicPatchSphere(void);
	void init(int baseFactor);
	virtual void draw(MvpShader s);
};

}
