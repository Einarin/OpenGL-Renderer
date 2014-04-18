#include "Mesh.h"
#include "VertexAttribBuilder.h"

enum flags{
	HAS_NORMAL = 0x1,
	HAS_TANGENT = 0x2
};

namespace gl{
struct MeshHeader{
	unsigned int nameoff;
	unsigned int vertoff;
	unsigned int vertsize;
	unsigned int indoff;
	unsigned int indsize;
	unsigned int flags;
	unsigned int drawCount;
	unsigned int numVertexColorChannels;
	unsigned int numUVChannels;
	unsigned int materialIndex;
	unsigned int numUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS];
};
uint32 Mesh::serialize(char** inbuff){
	uint32 bufflen = sizeof(MeshHeader)
			  + (name.size()+1)*sizeof(char)
			  + vertices.size() * sizeof(vertex)
			  + indices.size() * sizeof(unsigned int);
	char* buff = new char[bufflen];
	*inbuff = buff;
	MeshHeader* head = new(buff) MeshHeader;
	head->flags = hasNormals ? HAS_NORMAL : 0;
	head->flags |= hasTangents ? HAS_TANGENT : 0;
	head->drawCount = drawCount;
	head->numVertexColorChannels = numVertexColorChannels;
	head->numUVChannels = numUVChannels;
	head->materialIndex = materialIndex;
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		head->numUVComponents[i] = numUVComponents[i];
	}
	head->nameoff = sizeof(MeshHeader);
	strcpy((buff+head->nameoff),name.c_str());
	head->vertoff = sizeof(MeshHeader) + (name.size()+1)*sizeof(char);
	head->vertsize = vertices.size();
	memcpy(buff+head->vertoff,&vertices[0],vertices.size() * sizeof(vertex));
	head->indoff = sizeof(MeshHeader)
			  + (name.size()+1)*sizeof(char)
			  + vertices.size() * sizeof(vertex);
	head->indsize = indices.size();
	memcpy(buff+head->indoff,&indices[0],indices.size() * sizeof(unsigned int));
	return bufflen;
}

void Mesh::deserialize(char* buff){
	MeshHeader* head = reinterpret_cast<MeshHeader*>(buff);
	hasNormals = (head->flags & HAS_NORMAL) != 0;
	hasTangents = (head->flags & HAS_TANGENT) != 0;
	drawCount = head->drawCount;
	numVertexColorChannels = head->numVertexColorChannels;
	numUVChannels = head->numUVChannels;
	materialIndex = head->materialIndex;
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		numUVComponents[i] = head->numUVComponents[i];
	}
	name = buff+head->nameoff;
	vertices.resize(head->vertsize);
	memcpy(&vertices[0],buff+head->vertoff,head->vertsize * sizeof(vertex));
	indices.resize(head->indsize);
	memcpy(&indices[0],buff+head->indoff,head->indsize * sizeof(unsigned int));
}

void RenderableMesh::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	checkGlError("gen");
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkGlError("bind");
	VertexAttribBuilder b;
	b.setSize(sizeof(vertex))
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3).build();
	/*glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);*/
	checkGlError("attribs");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	checkGlError("init");
}

void RenderableMesh::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkGlError("glBindBuffer verts");
	glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vertex),&vertices[0],GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	checkGlError("glBindBuffer elements");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	checkGlError("glBufferData elements");
#ifdef _DEBUG
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
#endif
	checkGlError("download complete");
}

void RenderableMesh::draw(){
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,drawCount*3,GL_UNSIGNED_INT,(GLvoid*)0);
	checkGlError("drawelements");
	glBindVertexArray(0);
}

}//namespace gl