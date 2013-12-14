#include "Mesh.h"

namespace gl{
void RenderableMesh::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
}

void RenderableMesh::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
}

void RenderableMesh::draw(){
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,drawCount*3,GL_UNSIGNED_INT,(GLvoid*)0);
	checkGlError("drawelements");
	glBindVertexArray(0);
}

}//namespace gl