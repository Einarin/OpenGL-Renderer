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

void Billboard::position(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 botLeft, glm::vec2 botRight)
{
	corners[0].pos = vec4(topLeft,0,1);
    corners[1].pos = vec4(botLeft,0,1);
    corners[2].pos = vec4(topRight,0,1);
    corners[3].pos = vec4(botRight,0,1);
	download();
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
			vert.pos = vec3(x,y,z);
			vert.tc = vec3(texCoord.st,0.0);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)24);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)36);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	initialized = true;
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
	downloaded = true;
}

void IndexedGeometry::draw(){
	if(!initialized || !downloaded)
		return;
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

Cube::Cube(){
}

void Cube::generate(unsigned int tesselationFactor, glm::vec3 seed, bool simplexDisplace){
	m_displaced = simplexDisplace;
	tesselate(verts,indices,ivec3(tesselationFactor), seed,simplexDisplace);
}

void Cube::calcFaceNormal(std::vector<vertex>& verts,std::vector<unsigned int>& indices, int* pos)
{
	vec3 p1 = verts[pos[1]].pos.xyz - verts[pos[0]].pos.xyz;
	vec3 p2 = verts[pos[2]].pos.xyz - verts[pos[0]].pos.xyz;
	vec3 normal = normalize(cross(p1,p2));
	for(int i=0;i<3;i++){
		verts[pos[i]].normal.xyz += normal;
	}
}

void Cube::tesselate(std::vector<vertex>& verts,std::vector<unsigned int>& indices,ivec3 tesselationFactor, vec3 seed, bool displace){
	verts.resize(6*tesselationFactor.x*tesselationFactor.y);
	indices.resize(6*6*tesselationFactor.x*tesselationFactor.y);
	Future<bool> futures[6];
	for(int side=0;side<6;side++){
		futures[side] = glPool.async<bool>([=,&verts,&indices]()->bool{
		int vindex = side*tesselationFactor.x*tesselationFactor.y;
		int ind = side*6*tesselationFactor.x*tesselationFactor.y;
		int indsum = side*tesselationFactor.x*tesselationFactor.y;
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
				if(displace){
					int noisefactor = 0;//min(tesselationFactor.x,min(tesselationFactor.y,tesselationFactor.z));
					for(int i=0;i<(noisefactor/2);i++){
						//displacement += scale *(snoise(frequency * (position.xyz + seed)));
						displacement += 0.1f;// * snoise(position.xyz);
						frequency *=2;
						scale *= 0.5f;
					}
				}
				verts[vindex].pos = position*displacement;
				verts[vindex].normal = position;//vec3(0.0f,0.0f,0.0f);//vec4(position,1.0f);
				verts[vindex].tan = cross(y,position);
				//verts[vindex].tc = vec3(float(i)/float(tessX-1),float(j)/float(tessY-1),side/6.f);
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
					int pos[3] = {indsum + i*tessX + j, indsum + (i+1)*tessX + j, indsum + i*tessX + j+1};
					calcFaceNormal(verts,indices,pos);
					
					
					//triangle 2	 
					indices[ind++] = indsum + (i+1)*tessX + j;
					indices[ind++] = indsum + (i+1)*tessX + j+1;
					indices[ind++] = indsum + i*tessX + j+1;
					int pos2[3] = {indsum + (i+1)*tessX + j, indsum + (i+1)*tessX + j+1, indsum + i*tessX + j+1};
					calcFaceNormal(verts,indices,pos2);
				} else {
					//triangle 1
					indices[ind++] = indsum + i*tessX + j;
					indices[ind++] = indsum + i*tessX + j+1;
					indices[ind++] = indsum + (i+1)*tessX + j;
					int pos[3] = {indsum + i*tessX + j, indsum + i*tessX + j+1, indsum + (i+1)*tessX + j};
					calcFaceNormal(verts,indices,pos);
								 
					//triangle 2	 
					indices[ind++] = indsum + (i+1)*tessX + j;
					indices[ind++] = indsum + i*tessX + j+1;
					indices[ind++] = indsum + (i+1)*tessX + j+1;
					int pos2[3] = {indsum + (i+1)*tessX + j, indsum + i*tessX + j+1, indsum + (i+1)*tessX + j+1};
					calcFaceNormal(verts,indices,pos2);
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