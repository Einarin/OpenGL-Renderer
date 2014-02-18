#include "SkyBox.h"
#include "ImageLoader.h"
#include <glm/gtc/type_ptr.hpp>

namespace gl{

const float skyboxVerts[24] = {
	 1.f, -1.f, -1.f  ,
 1.f, -1.f, 1.f   ,
 -1.f, -1.f, 1.f  ,
 -1.f, -1.f, -1.f ,
 1.f, 1.f, -1.f   ,
 1.f, 1.f, 1.f    ,
 -1.f, 1.f, 1.f   ,
 -1.f, 1.f, -1.f  
};

const unsigned int vertCount = 24;

unsigned int skyboxIndices[36] = {
 1, 4, 3,
 1, 3, 2,
 5, 6, 7,
 5, 7, 8,
 1, 2, 6,
 1, 6, 5,
 2, 3, 7,
 2, 7, 6,
 3, 4, 8,
 3, 8, 7,
 4, 1, 5,
 4, 5, 8
};
const unsigned int indCount = 36;

const char* vert =
	"#version 330\n"
	"in vec3 in_position;\n"
	"out vec4 position;\n"
	"uniform mat4 viewMatrix;\n"
	"uniform mat4 projMatrix;\n"
	"void main(){\n"
	"position = vec4(in_position,1.0);\n"
	"gl_Position = projMatrix * viewMatrix * position;\n"
	"}\n";

const char* frag =
	"#version 330\n"
	"in vec4 position;\n"
	"out vec4 FragColor;\n"
	"uniform samplerCube cubemap;\n"
	"void main() {\n"
	"FragColor = texture(cubemap,position.xyz);\n"
	"}\n";

SkyBox::SkyBox(void):initialized(false),downloaded(false)
{
	cube.generate(20,glm::vec3(0));
}

SkyBox::~SkyBox(void)
{
}

void SkyBox::init()
{
	/*glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);*/
	cube.init();

	cubemap.init();

	bool shaderState = true;
	auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
	shaderState &= vs->compile(vert);
	auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
	shaderState &= fs->compile(frag);

	shader = Shader::Allocate();
	shader->addAttrib("in_position",0);
	shader->attachStage(vs);
	shader->attachStage(fs);
	shaderState &= shader->link();
	shader->bind();
	checkGlError("skybox shader");
	if(!shaderState) while(true); //shader compiler failure
	texloc = shader->getUniformLocation("cubemap");
	initialized = true;
}

void SkyBox::download(){
	/*glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,vertCount*sizeof(float),skyboxVerts,GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
	for(int i=0;i<indCount;i++)
		skyboxIndices[i]--;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	checkGlError("glBindBuffer elements");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indCount * sizeof(unsigned int), skyboxIndices, GL_STATIC_DRAW);
	checkGlError("glBufferData elements");
#ifdef _DEBUG
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
#endif*/
	cube.download();
	downloaded = true;
}

void SkyBox::setImage(std::string basepngfilename){
	glm::ivec2 imgSize;
	char* data;
	int bufflen;
	GLenum faces[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};
	char* suffixes[] = {"+x","-x","+y","-y","+z","-z"};
	cubemap.bind();
	for(int i=0;i<6;i++){
		bool success = imageDataFromPngFile(
			basepngfilename+suffixes[i]+".png",
			&imgSize,&data,&bufflen);
		cubemap.setup(GL_RGBA,imgSize,GL_UNSIGNED_BYTE,faces[i]);
		cubemap.setImage(GL_RGBA,imgSize,GL_UNSIGNED_BYTE,faces[i],data);
	}
}

void SkyBox::draw(Camera* c){
	if(!initialized || !downloaded)
		return;
	checkGlError("start draw");
	shader->bind();
	checkGlError("skybox shader");
	glUniformMatrix4fv(shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(c->GetViewMatrix()));
	glUniformMatrix4fv(shader->getUniformLocation("projMatrix"), 1, GL_FALSE, glm::value_ptr(c->GetProjectionMatrix()));
	checkGlError("mats");
	glActiveTexture(GL_TEXTURE0);
	cubemap.bind();
	glUniform1i(texloc,0);
	/*glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indCount,GL_UNSIGNED_INT,(GLvoid*)0);
	checkGlError("drawelements");
	glBindVertexArray(0);*/
	cube.draw();
}

}; //namespace gl
