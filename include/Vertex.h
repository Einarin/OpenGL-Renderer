#pragma once
#include "glincludes.h"
#include <assimp/mesh.h>

#define VERTEX_MAX_TEXCOORDS 1 //AI_MAX_NUMBER_OF_TEXTURECOORDS
#define VERTEX_MAX_TEXCOLORS 1 //AI_MAX_NUMBER_OF_COLOR_SETS
#define VERTEX_MAX_BONES 4

namespace gl{

	class vertex{
	public:
		glm::vec4 pos;
		glm::vec4 norm;
		glm::vec4 tan;
		glm::vec4 tc[VERTEX_MAX_TEXCOORDS];
		glm::vec4 colors[VERTEX_MAX_TEXCOLORS];
		int bones[VERTEX_MAX_BONES];
		float weights[VERTEX_MAX_BONES];
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