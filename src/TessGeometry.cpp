#include "TessGeometry.h"

using namespace glm;

namespace gl {

	static const vec3 normals[6] = {
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f)
	};

	static const vec3 corners[6][4] = {
		{// +X
			vec3(1.0f, -1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f)
		},
		{// +Y
			vec3(-1.0f, 1.0f, -1.0f),
			vec3(1.0f, 1.0f, -1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(-1.0f, 1.0f, 1.0f)
		},
		{// +Z
			vec3(-1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f,  1.0f),
			vec3(1.0f, -1.0f, 1.0f),
			vec3(-1.0f, -1.0f, 1.0f)
		},
		{// -X
			vec3(-1.0f, -1.0f, -1.0f),
			vec3(-1.0f, 1.0f, -1.0f),
			vec3(-1.0f, 1.0f, 1.0f),
			vec3(-1.0f, -1.0f, 1.0f)
		},
		{// -Y
			vec3(-1.0f, -1.0f, 1.0f),
			vec3(1.0f, -1.0f, 1.0f),
			vec3(1.0f, -1.0f, -1.0f),
			vec3(-1.0f, -1.0f, -1.0f)
		},
		{// -Z
			vec3(-1.0f, -1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f ),
			vec3(1.0f, 1.0f, -1.0f),
			vec3(-1.0f, 1.0f, -1.0f)
		},
	};

	Face::Face(int orientation) : m_orientation(orientation)
{}

	bool Face::tesselate(glm::mat4 modelMatrix, float lodFactor, Camera& c){
	//calculate distance from camera
	mat4 mvp = c.GetProjectionMatrix()*c.GetViewMatrix()*modelMatrix;
	vec3 distance = abs(c.GetPosition() - (mvp*vec4(normals[m_orientation],1.0f)).xyz() );
	//calculate direction relative to the camera
	float dotProduct = dot(normals[m_orientation], c.GetForwardVector());
	if (dotProduct > 0.707107f){ //if within 45 degrees of face pointing away from camera
		//assume invisible, and generate nothing
		//return false;
	}

	//calculate orientation in screen space
	

	VertexBufferBuilder buffBuilder;

	buffBuilder.vertexCount(4);
	buffBuilder.hasNormal(true);
	buffBuilder.hasTexCoord3D(true);
	VertexBuffer buffer = buffBuilder.build();

	for (int i = 0; i < 4; i++){
		buffer[i].pos() = corners[m_orientation][i];
		buffer[i].tc3(0) = normals[m_orientation];
		buffer[i].norm() = normals[m_orientation];
	}

	m_vao = VertexArrayObject();
	m_vao.bind();
	m_vao.addAndBindBuffer(GL_ARRAY_BUFFER);
	buffer.configureAttributes();
	glBufferData(GL_ARRAY_BUFFER, buffer.sizeInBytes(), buffer.buffPtr(), GL_DYNAMIC_DRAW);

	return true;
}

	void Face::draw(){
		m_vao.bind();
		glDrawArrays(GL_PATCHES, 0, 4);
	}

	TessCube::TessCube(){
		for (int i = 0; i < 6; i++){
			faces[i] = Face(i);
		}
	}

	void TessCube::tesselate(glm::mat4 modelMatrix, float lodFactor, Camera& c){
		for (Face& f : faces){
			f.tesselate(modelMatrix,lodFactor,c);
		}
	}

	void TessCube::draw(){
		for (Face& f : faces){
			f.draw();
		}
	}

} //namespace gl