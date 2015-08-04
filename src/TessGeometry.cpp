#include "TessGeometry.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static Matrix2D<gl::VertexArrayObject*> s_vaos(64,64);

namespace gl {
	static const mat4 faceTransforms[] = {
		mat4(),
		mat4(0.f, 0.f, -1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f),
		mat4(1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f),
		mat4(1.f, 0.f, 0.f, 0.f,
		0.f, -1.f, 0.f, 0.f,
		0.f, 0.f, -1.f, 0.f,
		0.f, 0.f, 0.f, 1.f),
		mat4(0.f, 0.f, -1.f, 0.f,
		0.f, -1.f, 0.f, 0.f,
		-1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f),
		mat4(1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, -1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f)
	};

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
	/*mat4 mvp = c.GetProjectionMatrix()*c.GetViewMatrix()*modelMatrix;
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
	glBufferData(GL_ARRAY_BUFFER, buffer.sizeInBytes(), buffer.buffPtr(), GL_DYNAMIC_DRAW);*/

		static int once;
		if (!once){
			if (buildLod(64))
				once++;
			//m_vao(s_vaos[2][2]);
		}

	return true;
}

	std::atomic_bool s_tessStarted;
	Future<bool> Face::IsTesselated;

	Future<bool> Face::buildLod(int maxTesselation) {
		bool started = true;
		started = s_tessStarted.exchange(started, std::memory_order_acq_rel);
		if (!started) {
			//CpuPool.async([maxTesselation]() {
				for (int xTess = 1; xTess < maxTesselation; xTess*=2) {
					for (int yTess = 1; yTess < maxTesselation; yTess*=2) {
						VertexBufferBuilder builder;
						builder.vertexCount((xTess + 1)*(yTess + 1));
						builder.hasNormal(true);
						builder.hasTexCoord3D(true);
						VertexBuffer* bufferp = new VertexBuffer();
						*bufferp = builder.build();
						VertexBuffer& buffer = *bufferp;
						int i = 0;
						for (int y = 0; y <= yTess; y++) {
							for (int x = 0; x <= xTess; x++) {
								buffer[i].pos() = vec3((float(2 * x) / float(xTess)) - 1.0f, (float(2 * y) / float(yTess)) - 1.0f, -1.f);
								buffer[i].tc3(0) = buffer[i].pos();
								buffer[i].norm() = glm::normalize(buffer[i].pos());
								i++;
							}
						}

						short* indexBuffer = new short[xTess*yTess * 4];
						i = 0;
						for (int x = 0; x < xTess; x++) {
							for (int y = 0; y < yTess; y++) {
								indexBuffer[i++] = x + y*(xTess + 1);
								indexBuffer[i++] = x + 1 + y*(xTess + 1);
								indexBuffer[i++] = x + 1 + (y + 1)*(yTess + 1);
								indexBuffer[i++] = x + (y + 1)*(yTess + 1);
							}
						}
						//std::cout << "before " << xTess << ", " << yTess << std::endl;
						//auto result = glQueue.async<void>([=]() {
							//std::cout << "uploading " << xTess << ", " << yTess << std::endl;
							auto pvao = new VertexArrayObject;
							s_vaos[xTess][yTess] = pvao;
							pvao->bind();
							pvao->addAndBindBuffer(GL_ARRAY_BUFFER);
							bufferp->configureAttributes();
							glBufferData(GL_ARRAY_BUFFER, bufferp->sizeInBytes(), bufferp->buffPtr(), GL_STATIC_DRAW);
							pvao->addAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER);
							glBufferData(GL_ELEMENT_ARRAY_BUFFER, xTess*yTess * 4 * sizeof(short), indexBuffer, GL_STATIC_DRAW);
							delete[] indexBuffer;
							delete bufferp;
						//});
						//result.wait();
						//glQueue.await(result);
						//std::cout << "after " << xTess << ", " << yTess << std::endl;
					}
				}
				IsTesselated = true;
			//});
		}
		return IsTesselated;
	}

	void Face::draw(ShaderRef s){
		//m_vao.bind();
		//glDrawArrays(GL_PATCHES, 0, 4*4);
		VertexArrayObject* obj = s_vaos[16][16];
		obj->bind();
		glUniformMatrix4fv(s->getUniformLocation("patchMatrix"),1, GL_FALSE, glm::value_ptr(faceTransforms[m_orientation]));
		glDrawElements(GL_PATCHES, 16*16*4, GL_UNSIGNED_SHORT, 0);
	}

	TessCube::TessCube(){
		for (int i = 0; i < 6; i++){
			faces[i] = Face(i);
		}
	}

	void TessCube::tesselate(glm::mat4 modelMatrix, float lodFactor, Camera& c){
		for (Face& f : faces){
			//f.tesselate(modelMatrix,lodFactor,c);
		}
	}

	void TessCube::draw(ShaderRef s){
		/*for (Face& f : faces){
			f.draw(s);
		}*/
		for (int i = 0; i < 6; i++) {
			if (Face::IsTesselated.isDone()) {
				faces[i].draw(s);
			}
		}
	}

} //namespace gl