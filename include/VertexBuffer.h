#include "VertexAttribBuilder.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace gl {

	class VertexBuffer {
	protected:
		VertexAttribBuilder m_attrib;
		std::shared_ptr<char> m_buffer;
		int m_vertSize;
		int m_posOffset;
		int m_normOffset;
		int m_tanOffset;
		std::vector<int> m_tcOffset;
		std::vector<int> m_colorOffset;
		
	public:
		class Vertex{
			friend class VertexBuffer;
		protected:
			Vertex(VertexBuffer vbuf, int index);
			m_hostbuffer;
		public:
			glm::vec3& pos();
			glm::vec3& norm();
			glm::vec3& tan();
			glm::vec2 tc2(int index);
			glm::vec3 tc3(int index);
			glm::vec4 color(int index);
		};
		friend class Vertex;

		Vertex operator[](int index);
	};

	class VertexBufferBuilder{
	public:
		VertexBufferBuilder& vertexCount(int count);
		VertexBufferBuilder& hasPosition(bool hasPos);
		VertexBufferBuilder& hasNormal(bool hasNorm);
		VertexBufferBuilder& hasTangent(bool hasTan);
		VertexBufferBuilder& hasTexCoord2D(int tcCount);
		VertexBufferBuilder& hasTexCoord3D(int tcCount);
		VertexBufferBuilder& hasTexCoord2D(int tcCount);
		VertexBufferBuilder& hasVertColor(int colorCount);
		VertexBuffer build();
	};
}