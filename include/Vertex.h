#pragma once
#include "glincludes.h"
#include <assimp/mesh.h>

//This file contains the definitions for vertex formats used

#define VERTEX_MAX_TEXCOORDS 1 //AI_MAX_NUMBER_OF_TEXTURECOORDS
#define VERTEX_MAX_TEXCOLORS 1 //AI_MAX_NUMBER_OF_COLOR_SETS

namespace gl{
	class VertexTextured{
	public:
		glm::vec3 pos;
		glm::vec2 tc[VERTEX_MAX_TEXCOORDS];
		static void configAttrib();
	};
	class VertexProcedural{
	public:
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec3 tan;
		static void configAttrib();
	};
	class VertexModel{
	public:
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec3 tan;
		glm::vec2 tc[VERTEX_MAX_TEXCOORDS];
		glm::vec4 colors[VERTEX_MAX_TEXCOLORS];
		static void configAttrib();
	};
	/*template<typename numType>
	class SmartVertexBuffer{
	protected:
		Mesh& mesh;
		unsigned int vertexSize;
		numType* buffer;
	public:
		SmartVertexBuffer(Mesh& target):mesh(target)
		{
			vertexSize=3;
			vertexSize += mesh.hasNormals? 3 : 0;
			vertexSize += mesh.hasTangents? 3: 0;
			for(int i=0;i<mesh.numUVChannels;i++){
				vertexSize += numUVComponents[i];
			}
			vertexSize += 4 * mesh.numVertexColorChannels;
			buffer = new T[vertexSize * mesh.vertexCount]
		}
		void set(vertex& vert, unsigned int index)
		operator void*(){
			return (void*)buffer;
		}
	};*/
}