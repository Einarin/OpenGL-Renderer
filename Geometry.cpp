#include "Geometry.h"
#include "Shader.h"
#include "noise.h"
#include "ThreadPool.h"
#include <iostream>

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

void StarBox::init()
{
	//geometry
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 16, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	//shader
	std::shared_ptr<ShaderStage> vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	vs->compile("attribute vec4 in_Position;\n"
"uniform mat4 viewProjMatrix;\n"
"void main(void)\n"
"{\n"\
"	gl_Position = viewProjMatrix * in_Position; \n"
"}\n");
	std::shared_ptr<ShaderStage> fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	starShader->attachStage(vs);
	starShader->attachStage(fs);
	starShader->link();
}

void StarBox::download()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,verts.size() * sizeof(vec4),&verts[0],GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
}

void StarBox::draw()
{
	starShader->bind();
	glBindVertexArray(vao);
	glDepthRangef(1.0f,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDepthRangef(0.0f,1.0f);
	glBindVertexArray(0);
}

StarBox::StarBox():starShader(Shader::Allocate())
{
	//front
	verts.push_back(vec4(vec3( -0.5, -0.5, -0.5),1.0));       // P1
	verts.push_back(vec4(vec3( -0.5,  0.5, -0.5),1.0));       // P2
	verts.push_back(vec4(vec3(  0.5,  0.5, -0.5),1.0));       // P3
	verts.push_back(vec4(vec3(  0.5, -0.5, -0.5),1.0));       // P4
	//back										,1.0
	verts.push_back(vec4(vec3(  0.5, -0.5, 0.5 ),1.0));
	verts.push_back(vec4(vec3(  0.5,  0.5, 0.5 ),1.0));
	verts.push_back(vec4(vec3( -0.5,  0.5, 0.5 ),1.0));
	verts.push_back(vec4(vec3( -0.5, -0.5, 0.5 ),1.0));
	//RIGHT										,1.0
	verts.push_back(vec4(vec3( 0.5, -0.5, -0.5 ),1.0));
	verts.push_back(vec4(vec3( 0.5,  0.5, -0.5 ),1.0));
	verts.push_back(vec4(vec3( 0.5,  0.5,  0.5 ),1.0));
	verts.push_back(vec4(vec3( 0.5, -0.5,  0.5 ),1.0));
	//LEFT
	verts.push_back(vec4(vec3( -0.5, -0.5,  0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5,  0.5,  0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5,  0.5, -0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5, -0.5, -0.5 ),1.0f));
	//TOP										 ,
	verts.push_back(vec4(vec3(  0.5,  0.5,  0.5 ),1.0f));
	verts.push_back(vec4(vec3(  0.5,  0.5, -0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5,  0.5, -0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5,  0.5,  0.5 ),1.0f));
	//BOTTOM									 ,
	verts.push_back(vec4(vec3(  0.5, -0.5, -0.5 ),1.0f));
	verts.push_back(vec4(vec3(  0.5, -0.5,  0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5, -0.5,  0.5 ),1.0f));
	verts.push_back(vec4(vec3( -0.5, -0.5, -0.5 ),1.0f));
}

Sphere::Sphere(unsigned int tesselationFactor){
	tesselate(verts,indices,tesselationFactor,vec2(0.0),vec2(1.0));
}

Sphere::Sphere(unsigned int tesselationFactor,vec2 start, vec2 end){
	tesselate(verts,indices,tesselationFactor,start,end);
}

void Sphere::tesselate(std::vector<vertex>& verts,std::vector<unsigned int>& indices,unsigned int tesselationFactor,vec2 start, vec2 end){
	verts.resize(tesselationFactor * tesselationFactor);
	for(int i=0;i<(int)tesselationFactor;i++){
		for(int j=0;j<(int)tesselationFactor;j++){
			//x^2 + y^2 + z^2 = 1;
			vec2 texCoord = vec2(float(j)/(tesselationFactor-1),float(i)/float(tesselationFactor-1))*(end-start) + start;
			float y = cos(3.1415f * texCoord.y);
			float xzMag = sqrt(1-y*y);
			float angle = texCoord.x * 2.f * 3.1415f;
			float x = cos(angle) * xzMag;
			float z = sin(angle) * xzMag;
			vertex& vert = verts[i*tesselationFactor + j];
			vert.pos = vec4(x,y,z,1.0f);
			vert.tc = vec4(texCoord.st,0.f,1.f);
			vert.normal = vert.pos;
		}
	}
	unsigned int triangles = 2 * (tesselationFactor) * (tesselationFactor - 1);
	indices.resize(3 * triangles);
	unsigned int ind = 0;
	for(int i=0;i<(int)tesselationFactor-1;i++){
		for(int j=0;j<(int)tesselationFactor-1;j++){
			//triangle 1
			indices[ind++] = i*tesselationFactor + j;
			indices[ind++] = i*tesselationFactor + j+1;
			indices[ind++] = (i+1)*tesselationFactor + j;
			
			//triangle 2
			indices[ind++] = (i+1)*tesselationFactor + j;
			indices[ind++] = i*tesselationFactor + j+1;
			indices[ind++] = (i+1)*tesselationFactor + j+1;
		}
		//last triangle pair to complete the ring
		/*indices[ind++] = i*tesselationFactor + tesselationFactor-1;
		indices[ind++] = (i+1)*tesselationFactor + tesselationFactor-1;
		indices[ind++] = i*tesselationFactor;
		//triangle 2
		indices[ind++] = (i+1)*tesselationFactor + tesselationFactor-1;
		indices[ind++] = (i+1)*tesselationFactor;
		indices[ind++] = i*tesselationFactor;*/
	}
}

void IndexedGeometry::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)32);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
}

void IndexedGeometry::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,verts.size() * sizeof(vertex),&verts[0],GL_STATIC_DRAW);
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

void IndexedGeometry::draw(){
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,(GLvoid*)0);
	checkGlError("drawelements");
	glBindVertexArray(0);
}

int cubeSides[6][3][3] = {
	{{1,0,0},{0,1,0},{0,0,1}},
	{{0,1,0},{0,0,1},{1,0,0}},
	{{0,1,0},{0,0,1},{-1,0,0}},
	{{1,0,0},{0,0,1},{0,1,0}},
	{{1,0,0},{0,0,1},{0,-1,0}},
	{{1,0,0},{0,1,0},{0,0,-1}}
};

Cube::Cube(unsigned int tesselationFactor){
	tesselate(verts,indices,ivec3(tesselationFactor));
}

void Cube::tesselate(std::vector<vertex>& verts,std::vector<unsigned int>& indices,ivec3 tesselationFactor){
	verts.resize(6*tesselationFactor.x*tesselationFactor.y);
	indices.resize(6*6*tesselationFactor.x*tesselationFactor.y);
	Future<bool> futures[6];
	for(int side=0;side<6;side++){
		futures[side] = glPool.async<bool>([=,&verts,&indices]()->bool{
		int vindex = side*tesselationFactor.x*tesselationFactor.y;
		int ind = side*6*tesselationFactor.x*tesselationFactor.y;
		int indsum = max(0,(side-1)*tesselationFactor.x*tesselationFactor.y);
		ivec3 ix(cubeSides[side][0][0],cubeSides[side][0][1],cubeSides[side][0][2]);
		ivec3 iy(cubeSides[side][1][0],cubeSides[side][1][1],cubeSides[side][1][2]);
		vec3 z(cubeSides[side][2][0],cubeSides[side][2][1],cubeSides[side][2][2]);
		ivec3 xl = ix*tesselationFactor;
		ivec3 yl = iy*tesselationFactor;
		int tessX = xl.x+xl.y+xl.z;
		int tessY = yl.x+yl.y+yl.z;
		vec3 x(cubeSides[side][0][0],cubeSides[side][0][1],cubeSides[side][0][2]);
		vec3 y(cubeSides[side][1][0],cubeSides[side][1][1],cubeSides[side][1][2]);
		
		//vertex buffer
		for(int i=0;i<tessX;i++){
			for(int j=0;j<tessY;j++){
				vec3 position = vec3((x*(2.f*float(i)/float(tessX-1)-1.f) +  y*(2.f*float(j)/float(tessY-1)-1.f) + z));
				position = normalize(position);
				float scale = 0.1f;////clamp(time,0.0,1.0);
				float frequency = 1.0f;
				float displacement = 1.0f;
				for(int i=0;i<4;i++){
					displacement += scale *(snoise(frequency * position.xyz));
					frequency *= 10;
					scale *= 0.1f;
				}
				verts[vindex].pos = vec4(position*displacement,1.0f);
				verts[vindex].normal = vec4(position,1.0f);
				vindex++;
				/*verts[index].pos = vec4(x*(float(2*(i+1)-1)/float(tessX)) + y*(float(2*j-1)/float(tessY))+z,1.0);
				index++;
				verts[index].pos = vec4(x*(float(2*i-1)/float(tessX)) + y*(float(2*(j+1)-1)/float(tessY))+z,1.0);
				index++;
				verts[index].pos = vec4(x*(float(2*(i+1)-1)/float(tessX)) + y*(float(2*(j+1)-1)/float(tessY))+z,1.0);
				index++;*/
			}
		}
		//index buffer
		for(int i=0;i<tessX-1;i++){
			for(int j=0;j<tessY-1;j++){
				//half the sides need the indices in opposite order so the triangle front face is outwards :P
				if(side == 0 || side == 1 || side == 4){	 
					//triangle 1
					indices[ind++] = indsum + i*tessX + j;
					indices[ind++] = indsum + (i+1)*tessX + j;
					indices[ind++] = indsum + i*tessX + j+1;
					
					//triangle 2	 
					indices[ind++] = indsum + (i+1)*tessX + j;
					indices[ind++] = indsum + (i+1)*tessX + j+1;
					indices[ind++] = indsum + i*tessX + j+1;
				} else {
					//triangle 1
					indices[ind++] = indsum + i*tessX + j;
					indices[ind++] = indsum + i*tessX + j+1;
					indices[ind++] = indsum + (i+1)*tessX + j;
								 
					//triangle 2	 
					indices[ind++] = indsum + (i+1)*tessX + j;
					indices[ind++] = indsum + i*tessX + j+1;
					indices[ind++] = indsum + (i+1)*tessX + j+1;
				}
			}
		}
		indsum = vindex;
		std::cout << "side " << side << " ";
		return true;
		});
	}
	bool success = true;
	for(int i=0;i<6;i++){
		success &= glPool.await(futures[i]);
	}
}

} //namespace gl