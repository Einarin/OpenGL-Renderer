#include "Geometry.h"

namespace gl{

using namespace glm;

Billboard::Billboard(){
	corners[0].pos = vec4(-1,-1,0,1);
    corners[0].txc = vec4(0,1,0,1);
    corners[1].pos = vec4(1,-1,0,1);
    corners[1].txc = vec4(1,1,0,1);
    corners[2].pos = vec4(-1,1,0,1);
    corners[2].txc = vec4(0,0,0,1);
    corners[3].pos = vec4(1,1,0,1);
    corners[3].txc = vec4(1,0,0,1);
}

Billboard::Billboard(vec3 topLeft, vec3 topRight, vec3 botLeft, vec3 botRight)
{
    corners[0].pos = vec4(topLeft,1);
    corners[0].txc = vec4(0,1,0,1);
    corners[1].pos = vec4(botLeft,1);
    corners[1].txc = vec4(1,1,0,1);
    corners[2].pos = vec4(topRight,1);
    corners[2].txc = vec4(0,0,0,1);
    corners[3].pos = vec4(botRight,1);
    corners[3].txc = vec4(1,0,0,1);
}

Billboard::Billboard(float topLeftX, float topLeftY, float width, float height)
{
    corners[0].pos = vec4(topLeftX,topLeftY,0,1);
    corners[0].txc = vec4(0,1,0,1);
    corners[1].pos = vec4(topLeftX,topLeftY+height,0,1);
    corners[1].txc = vec4(1,1,0,1);
    corners[2].pos = vec4(topLeftX+width,topLeftY,0,1);
    corners[2].txc = vec4(0,0,0,1);
    corners[3].pos = vec4(topLeftX+width,topLeftY+height,0,1);
    corners[3].txc = vec4(1,0,0,1);
}

void Billboard::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 32, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 32, (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Billboard::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,4 * sizeof(vertex),corners,GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
}

void Billboard::move(float topLeftX, float topLeftY, float width, float height)
{
	corners[0].pos = vec4(topLeftX,topLeftY,0,1);
    corners[1].pos = vec4(topLeftX,topLeftY+height,0,1);
    corners[2].pos = vec4(topLeftX+width,topLeftY,0,1);
    corners[3].pos = vec4(topLeftX+width,topLeftY+height,0,1);
	download();
}
void Billboard::move(float topLeftX, float topLeftY)
{
	vec2 size = corners[3].pos.xy - corners[0].pos.xy;
	corners[0].pos = vec4(topLeftX,topLeftY,0,1);
    corners[1].pos = vec4(topLeftX,topLeftY+size.y,0,1);
    corners[2].pos = vec4(topLeftX+size.x,topLeftY,0,1);
    corners[3].pos = vec4(topLeftX+size.x,topLeftY+size.y,0,1);
	download();
}
void Billboard::moveRel(float dx, float dy)
{
	corners[0].pos.xy += vec2(dx,dy);
	corners[1].pos.xy += vec2(dx,dy);
	corners[2].pos.xy += vec2(dx,dy);
	corners[3].pos.xy += vec2(dx,dy);
	download();
}

void Billboard::draw(){
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

Billboard::~Billboard()
{
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
}

} //namespace gl