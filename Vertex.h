#pragma once
#include "glincludes.h"
#include <assimp/mesh.h>

#define VERTEX_MAX_TEXCOORDS 1 //AI_MAX_NUMBER_OF_TEXTURECOORDS
#define VERTEX_MAX_TEXCOLORS 1 //AI_MAX_NUMBER_OF_COLOR_SETS

namespace gl{
	using glm::vec3;
	using glm::vec4;
	class vertex{
	public:
		vec3 pos;
		vec3 tan;
		vec3 norm;
		vec3 tc[VERTEX_MAX_TEXCOORDS];
		vec4 colors[VERTEX_MAX_TEXCOLORS];
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