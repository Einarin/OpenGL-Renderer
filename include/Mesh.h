#pragma once
#include "VertexBuffer.h"
#include "Geometry.h"
#include "AABB.h"
#include <vector>
#include <string>
#include <assimp/mesh.h>

namespace gl{

	class Bone{
	public:
		std::string name;
	};

	class Mesh{
	public:
		struct MeshHeader{
			unsigned int nameoff;
			unsigned int vertoff;
			unsigned int vertsize; //this isn't strictly needed but it's used to verify the computed length calculated by VertexBuffer matches the length of the stored data
			unsigned int vertCount;
			unsigned int indoff;
			unsigned int indsize;
			unsigned int flags;
			unsigned int drawCount;
			unsigned int numVertexColorChannels;
			unsigned int numUVChannels;
			unsigned int materialIndex;
			unsigned int numUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		};
		std::string name;
		VertexBuffer vertices;
		unsigned int vertSize;
		unsigned int* indices;
		unsigned int indSize;
		AABB3 BoundingBox;
		bool ownsBuffers;
		bool hasNormals;
		bool hasTangents;
		bool hasBones;
		unsigned int drawCount;
		unsigned int numVertexColorChannels;
		unsigned int numUVChannels;
		unsigned int materialIndex;
		unsigned int numUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		Mesh():vertSize(0),indSize(0),ownsBuffers(false) //partial initialization
		{}
		~Mesh(){
			if(ownsBuffers){
				delete[] indices;
			}
		}
		virtual uint32 serialize(char** inbuff);
		bool deserialize(char* buf); //returns false if we can't make sense of the buffer
	private:
		//assumes we don't own any buffers!
		void copy(const Mesh& other);
	public:
        const Mesh& operator=(const Mesh& other){
			if(ownsBuffers){
				delete[] indices;
			}
			copy(other);
			return other;
		}
		Mesh(const Mesh& other):vertSize(0),indSize(0){
			copy(other);
		}
		void calcAABB();
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
