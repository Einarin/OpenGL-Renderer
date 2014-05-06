#include "DynamicPatchSphere.h"
#include "glincludes.h"
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

using namespace glm;

namespace gl{
DynamicPatchSphere::DynamicPatchSphere(void)
{
	for(int i=0;i<6;i++){
		facePatchCount[i] = 0;
	}
}


DynamicPatchSphere::~DynamicPatchSphere(void)
{
	for(int i=0;i<6;i++){
		if(facePatchCount[i] > 0)
			delete[] facePatches[i];
	}
}
std::function<vec3(vec3)> transform[] = {
	[](vec3 in)->vec3{
		return glm::vec3(in.x,in.y,1.f);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,in.y,-1.f);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-1.f,in.x,-in.y);
	},
	[](vec3 in)->vec3{
		return glm::vec3(1.f,in.x,in.y);
	},
	[](vec3 in)->vec3{
		return glm::vec3(in.x,1.f,-in.y);
	},
	[](vec3 in)->vec3{
		return glm::vec3(in.x,-1.f,in.y);
	}
};

std::function<vec3(vec3)> transform2[] = {
	[](vec3 in)->vec3{
		return in;
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,1.f-in.y,in.z);
	},
	[](vec3 in)->vec3{
		return glm::vec3(1.f-in.x,-in.y,in.z);
	},
	[](vec3 in)->vec3{
		return glm::vec3(-in.x,-in.y,in.z);
	}
};
void DynamicPatchSphere::init(int baseFactor){
	for(int i=0;i<12;i++){
		m_edgeTessFactors[i] = baseFactor;
	}
	for(int i=0;i<6;i++){
		facePatches[i] = new Patch[4];
		facePatchCount[i] = 4;
		for(int j=0;j<4;j++){
			int tessFactor[] = {baseFactor,baseFactor,baseFactor,baseFactor};
			facePatches[i][j].tesselate(tessFactor,[=](vec3 in)->vec3{
				return normalize(transform[i](transform2[j](in)));
			});
			facePatches[i][j].init();
			facePatches[i][j].download();
		}
	}
}

void DynamicPatchSphere::draw(MvpShader s){
	s.setModel(mat4());
	for(int i=0;i<6;i++){
		for(int j=0;j<facePatchCount[i];j++){
			facePatches[i][j].draw();
		}
	}
}

}
