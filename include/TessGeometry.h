#include "glincludes.h"
#include "Shader.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "VertexArrayObject.h"

namespace gl{

	class Face{
	protected:
		int m_orientation;
		//VertexBuffer m_buffer;
		VertexArrayObject m_vao;
	public:
		inline Face()
		{}
		Face(int orientation);
		bool tesselate(glm::mat4 modelMatrix, float lodFactor, Camera& c);
		void draw();
	};

	class TessCube{
	protected:
		Face faces[6];
	public:
		TessCube();
		void tesselate(glm::mat4 modelMatrix, float lodFactor, Camera& c);
		void draw();
	};
}