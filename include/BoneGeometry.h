#pragma once
#include "glincludes.h"
#include "VertexBuffer.h"

namespace gl {
	class BoneGeometry {
	private:
		GLuint m_vbo;
		GLuint m_ibo;
		GLuint m_vao;
	public:
		void download() {
			GLuint two[2];
			glGenBuffers(2, two);
			m_vbo = two[0];
			m_ibo = two[1];
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			VertexBufferBuilder vbb;
			vbb.hasNormal(true);
			vbb.vertexCount(5);
			VertexBuffer vb = vbb.build();
			using glm::vec3;
			vb[0].pos() = vec3(0.f, 0.f, 0.f);
			vb[0].norm() = vec3(-1.f, 0.f, 0.f);
			vb[1].pos() = vec3(0.33f, 0.1f, 0.f);
			vb[1].norm() = vec3(0.f, 1.f, 0.f);
			vb[2].pos() = vec3(0.33f, -0.033f, 0.067f);
			vb[2].norm() = vec3(0.f, -0.33f, 0.67f);
			vb[3].pos() = vec3(0.33f, -0.033f, -0.067f);
			vb[3].norm() = vec3(0.f, -0.33f, -0.67f);
			vb[4].pos() = vec3(1.f, 0.f, 0.f);
			vb[4].norm() = vec3(1.f, 0.f, 0.f);
			glBufferData(GL_ARRAY_BUFFER, vb.sizeInBytes(), vb.buffPtr(), GL_STATIC_DRAW);
			vb.configureAttributes();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			GLubyte ib[] = {
				0,2,1,
				2,0,3,
				0,1,3,
				1,2,4,
				2,3,4,
				4,3,1
			};
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 3, ib, GL_STATIC_DRAW);
			glBindVertexArray(0);
		}
		void draw() {
			glBindVertexArray(m_vao);
			glDrawElements(GL_TRIANGLES, 6*3, GL_UNSIGNED_BYTE, nullptr);
			glBindVertexArray(0);
		}
	};
}