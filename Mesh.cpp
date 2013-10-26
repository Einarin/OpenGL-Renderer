#include "Mesh.h"

namespace gl{
void RenderableMesh::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void RenderableMesh::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,vertexCount * sizeof(vertex),&vertices[0],GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
}

void RenderableMesh::draw(){
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 4);
	glBindVertexArray(0);
}

}//namespace gl