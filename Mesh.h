#pragma once
#include "Vertex.h"
#include "Geometry.h"
#include <vector>

namespace gl{

	class Mesh{
	public:
		std::vector<vertex> vertices;
		bool hasNormals;
		bool hasTangents;
		unsigned int vertexCount;
		unsigned int numVertexColorChannels;
		unsigned int numUVChannels;
		unsigned int materialIndex;
		unsigned int numUVComponents[VERTEX_MAX_TEXCOORDS];
	};

	class RenderableMesh : public Mesh, public Geometry{
	protected:
		unsigned int vao;
		unsigned int vbo;
	public:
		virtual void init();
        virtual void download();
        virtual void draw();
	};

}