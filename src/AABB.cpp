#include "glincludes.h"
#include "AABB.h"
#include "VertexAttribBuilder.h"
#include "Shader.h"

namespace gl{

AABB3&& operator*(glm::mat4 transform, const AABB3& aabb){
	glm::vec3 oldMin = (transform * glm::vec4(aabb.mMinCorner,1.0f)).xyz();
	glm::vec3 oldMax = (transform * glm::vec4(aabb.mMaxCorner,1.0f)).xyz();
	return AABB3(glm::min(oldMin,oldMax),glm::max(oldMin,oldMax));
}


const unsigned char drawInds[] = {
	0,1, 0,2, 1,3, 2,3,
	0,4, 1,5, 2,6, 3,7,
	4,5, 4,6, 5,7, 6,7
};
void buildAndRender(AABB3 aabb){
	//if(mEmpty) return;
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

void drawAABB3(const AABB3& aabb){
	
	
	

}

}