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
		unsigned int numUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		virtual uint32 serialize(char** inbuff);
		void deserialize(char* buf);
	};

	class RenderableMesh : public Mesh{
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