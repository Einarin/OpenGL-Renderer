#include "glincludes.h"
#include "AABB.h"
#include "VertexAttribBuilder.h"
#include "Shader.h"

namespace gl{

AABB3 operator*(glm::mat4 transform, const AABB3& aabb){
	BB3 tmp = BB3fromAABB3(aabb);
	tmp = transform * tmp;
	return AABB3fromBB3(tmp);
}

BB3 operator*(glm::mat4 transform, const BB3& bb){
	BB3 result;
	for(int i=0;i<8;i++){
		result.corners[i] = (transform * glm::vec4(bb.corners[i],1.0f)).xyz();
	}
	result.empty = bb.empty;
	return result;
}

const unsigned char drawInds[] = {
	0,1, 0,2, 1,3, 2,3,
	0,4, 1,5, 2,6, 3,7,
	4,5, 4,6, 5,7, 6,7
};
void buildAndRender(AABB3 aabb){
	VertexArrayObject vao;
	vao.bind();
	vao.addAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,24, drawInds, GL_STREAM_DRAW);
	vao.addAndBindBuffer(GL_ARRAY_BUFFER);
	VertexAttribBuilder b;
	b.attrib(FLOAT_ATTRIB,3).build();
	float data[] = {
		aabb.mMinCorner.x, aabb.mMinCorner.y, aabb.mMinCorner.z, 
		aabb.mMinCorner.x, aabb.mMinCorner.y, aabb.mMaxCorner.z, 
		aabb.mMinCorner.x, aabb.mMaxCorner.y, aabb.mMinCorner.z, 
		aabb.mMinCorner.x, aabb.mMaxCorner.y, aabb.mMaxCorner.z, 
		aabb.mMaxCorner.x, aabb.mMinCorner.y, aabb.mMinCorner.z, 
		aabb.mMaxCorner.x, aabb.mMinCorner.y, aabb.mMaxCorner.z, 
		aabb.mMaxCorner.x, aabb.mMaxCorner.y, aabb.mMinCorner.z, 
		aabb.mMaxCorner.x, aabb.mMaxCorner.y, aabb.mMaxCorner.z
	};
	glBufferData(GL_ARRAY_BUFFER,sizeof(data), data, GL_STREAM_DRAW);
	glDrawElements(GL_LINES,24,GL_UNSIGNED_BYTE,(GLvoid*)0);
}

BB3 BB3fromAABB3(const AABB3& aabb){
	BB3 bb;
	bb.corners[0] =	glm::vec3(aabb.mMinCorner.x, aabb.mMinCorner.y, aabb.mMinCorner.z);
	bb.corners[1] =	glm::vec3(aabb.mMinCorner.x, aabb.mMinCorner.y, aabb.mMaxCorner.z);
	bb.corners[2] =	glm::vec3(aabb.mMinCorner.x, aabb.mMaxCorner.y, aabb.mMinCorner.z);
	bb.corners[3] =	glm::vec3(aabb.mMinCorner.x, aabb.mMaxCorner.y, aabb.mMaxCorner.z);
	bb.corners[4] =	glm::vec3(aabb.mMaxCorner.x, aabb.mMinCorner.y, aabb.mMinCorner.z);
	bb.corners[5] =	glm::vec3(aabb.mMaxCorner.x, aabb.mMinCorner.y, aabb.mMaxCorner.z);
	bb.corners[6] =	glm::vec3(aabb.mMaxCorner.x, aabb.mMaxCorner.y, aabb.mMinCorner.z);
	bb.corners[7] =	glm::vec3(aabb.mMaxCorner.x, aabb.mMaxCorner.y, aabb.mMaxCorner.z);
	bb.empty = false;
	return bb;
}

AABB3 AABB3fromBB3(const BB3& bb){
	glm::vec3 minp,maxp;
	minp = maxp = bb.corners[0];
	for(int i=1;i<8;i++){
		minp = glm::min(minp,bb.corners[i]);
		maxp = glm::max(maxp,bb.corners[i]);
	}
	return AABB3(minp,maxp);
}

}