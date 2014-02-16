#pragma once
#include "Vertex.h"
#include "Geometry.h"
#include <vector>
#include <string>

namespace gl{

	class Mesh{
	public:
		std::string name;
		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;
		bool hasNormals;
		bool hasTangents;
		unsigned int drawCount;
		unsigned int numVertexColorChannels;
		unsigned int numUVChannels;
		unsigned int materialIndex;
		unsigned int numUVComponents[VERTEX_MAX_TEXCOORDS];
		virtual uint32 serialize(char** inbuff);
		void deserialize(char* buf);
	};

	class RenderableMesh : public Mesh, public Geometry{
	protected:
		unsigned int vao;
		unsigned int vbo;
		unsigned int ibo;
	public:
		virtual void init();
        virtual void download();
        virtual void draw();
	};

}