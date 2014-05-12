#include "Texture.h"
#include "ImageLoader.h"
#include <iostream>
#include "Shader.h"
#include "Geometry.h"

namespace gl {

using glm::ivec2;

//TODO: DirectX support?
GlTextureManager manager;

void GlTexture::setInterpolation(){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
}
void GlTexture::setMapping(){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mapMode );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mapMode );
}
void GlTexture::linearInterpolation()
{
	interpolation = GL_LINEAR;
	if(allocated){
		bind();
		setInterpolation();
	}
}

void GlTexture::nearestInterpolation()
{
	interpolation = GL_NEAREST;
	if(allocated){
		bind();
		setInterpolation();
	}
}

GlTexture::GlTexture():id(0),backed(false),
	interpolation(GL_LINEAR),mapMode(GL_CLAMP)
{}

GlTexture2D::GlTexture2D()
{
	type = GL_TEXTURE_2D;
}

GlTexture2D::GlTexture2D(unsigned int texId)
{
	id = texId;
	type = GL_TEXTURE_2D;
}

void GlTexture2D::alloc(){
	allocated=true;
	glGenTextures(1,&id);
	bind();
	setInterpolation();
	setMapping();
	checkGlError("GlTexture2D::alloc()");
}

void GlTexture2D::dealloc(){
	glDeleteTextures(1,&id);
	allocated=false;
}

void GlTexture::init()
{
	if(!allocated)
	{
		alloc();
	}
	setInterpolation();
	backed = false;
}
void GlTexture::bind()
{
    glBindTexture(type, id);
}
void GlTexture::unbind()
{
    glBindTexture(type, 0);
}
void GlTexture::wrap(){
	mapMode=GL_REPEAT;
}
void GlTexture::clamp(){
	mapMode=GL_CLAMP;
}
void GlTexture::mirror(){
	mapMode=GL_MIRRORED_REPEAT;
}
void GlTexture2D::setup(GLint format,ivec2 texSize,GLenum datatype)
{
	if(!allocated){
		alloc();
	}
	size = texSize;
    glBindTexture(type, id);
    glTexImage2D(type,0,format,size.x,size.y,0,format,datatype,0);
	checkGlError("GlTexture2D::setup()");
    backed = true;
}
void GlTexture2D::setImage(GLint format,ivec2 texSize,GLenum datatype,void* data)
{
	if(!backed || size != texSize){
		setup(format,texSize,datatype);
	}
	if(data != 0) {
		//check the data so we crash instead of the driver
		unsigned int *ptr = (unsigned int*)data;
		//If you crash here we got fed a bad image pointer!

		//printGlDebug("Begin crash test %u, %u ", size.x, size.y);
		//volatile int x = ptr[0];
		//volatile int y = ptr[size.x*size.y - 1];
		//printGlDebug("End crash test");

		glBindTexture(GL_TEXTURE_2D, id);
		checkGlError("glBindTexture");
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,size.x,size.y,format,datatype,data);
		checkGlError("glTexSubImage2D");
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		//texture is NULL
	}
}

void GlTexture2D::draw(){
	static int setup;
	static Billboard bb;
	static ShaderRef shader;
	if(setup){
		shader->bind();
	} else {
		shader = Shader::Allocate();
		auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
		auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
		vs->compile("#version 330\n"
					"in vec2 position;\n"
					"out vec2 texCoord;\n"
					"void main(void){\n"
					"texCoord = 0.5*(position+1.0);\n"
					"gl_Position = vec4(position,0.0,1.0);\n"
					"}\n");
		fs->compile("#version 330\n"
			"in vec2 texCoord;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D tex;\n"
			"void main(void){\n"
			"vec4 frag = texture(tex,texCoord);\n"
			"FragColor = vec4(frag.rgb,1.0);\n"
			"}\n");
		shader->attachStage(vs);
		shader->attachStage(fs);
		shader->link();
		shader->bind();
		glUniform1i(shader->getUniformLocation("tex"),0);
		bb.init();
		bb.download();
		setup = 1;
	}
	glActiveTexture(GL_TEXTURE0);
	bind();
	bb.draw();
}

void GlTexture2DDepth::draw(){
	static int setup;
	static Billboard bb;
	static ShaderRef shader;
	if(setup){
		shader->bind();
	} else {
		shader = Shader::Allocate();
		auto vs = ShaderStage::Allocate(GL_VERTEX_SHADER);
		auto fs = ShaderStage::Allocate(GL_FRAGMENT_SHADER);
		vs->compile("#version 330\n"
					"in vec2 position;\n"
					"out vec2 texCoord;\n"
					"void main(void){\n"
					"texCoord = 0.5*(position+1.0);\n"
					"gl_Position = vec4(position,0.0,1.0);\n"
					"}\n");
		fs->compile("#version 330\n"
			"in vec2 texCoord;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D tex;\n"
			"void main(void){\n"
			"float frag = texture(tex,texCoord).s;\n"
			"frag = frag/18.0 - 2.0;\n"
			"FragColor = vec4(vec3(frag),0.5);\n"
			"}\n");
		shader->attachStage(vs);
		shader->attachStage(fs);
		shader->link();
		shader->bind();
		glUniform1i(shader->getUniformLocation("tex"),0);
		bb.init();
		bb.download();
		setup = 1;
	}
	glActiveTexture(GL_TEXTURE0);
	bind();
	bb.draw();
}

GlTexture::~GlTexture() {
	if(getId() > 0) //If id is zero we never allocated anything in OpenGL, don't print anything
	glDeleteTextures(1,&id);
}

unsigned int GlTexture::getId()
{
	return id;
}

TextureManager* TextureManager::Instance()
{
	//TODO: DirectX support?
	return &manager;
}

void GlTexture2D::unreferenced(){
	//TODO: something?
}

TexRef GlTextureManager::texFromFile(std::string file){
	FileBackedGlTexture2D* tex = new FileBackedGlTexture2D();
	tex->filename = file;
	return TexRef(tex);
}
TexRef GlTextureManager::unbackedTex(){
	return TexRef(new GlTexture2D());
}
TexRef GlTextureManager::backedTex(unsigned int format,glm::ivec2 size, unsigned int datatype){
	auto ptr = new GlTexture2D();
	ptr->init();
	ptr->setup(format,size,datatype);
	return TexRef(ptr);
}
TexRef GlTextureManager::depthTex(unsigned int format,glm::ivec2 size, unsigned int datatype){
	auto ptr = new GlTexture2DDepth();
	ptr->init();
	ptr->setup(format,size,datatype);
	return TexRef(ptr);
}
static TexRef defaultTex;
TexRef GlTextureManager::missingTex(){
	if(defaultTex.use_count() == 0){
		auto ptr = new GlTexture2D();
		defaultTex = std::shared_ptr<GlTexture2D>(ptr);
		ptr->init();
		unsigned char data[] = {
			255,0,255,
			0,255,0,
			0,255,0,
			255,0,255};
		ptr->setImage(GL_RGB,ivec2(2,2),GL_UNSIGNED_BYTE,data);
		ptr->nearestInterpolation();
	}
	return defaultTex;
}
void GlTextureManager::contextLost()
{
	//TODO: need to call init on all Textures when the context is lost
}
TexRef GlTextureManager::texFromPngBytestream(char* buff, int size){
	GlTexture2D* tex = new GlTexture2D();
	if(!loadGlTex2dFromPngBytestream(tex,buff,size)){
		std::cout << "Loading image from PNG bytestream failed, texture will be invalid\n";
	}
	return TexRef(tex);
}
TexRef GlTextureManager::texFromRGBA8888(char* buff, glm::ivec2 size){
	GlTexture2D* tex = new GlTexture2D();
	tex->setImage(GL_RGBA8,size,GL_UNSIGNED_BYTE,(void*)buff);
	return TexRef(tex);
}
void FileBackedGlTexture2D::init(){
	GlTexture2D::init();
	if(!backed){
		if(!loadPngToGlTex2D(filename,(GlTexture2D*)this))
			std::cout << "Loading " << filename << " failed\n";
		checkGlError("loadPngToGlTex2D");
	}
}
void FileBackedGlTexture2D::bind(){
	if(!allocated || !backed){
		init();
	}
	GlTexture2D::bind();
	checkGlError("binding texture "+filename);
}

GlTextureCubeMap::GlTextureCubeMap()
{
	type = GL_TEXTURE_CUBE_MAP;
}

GlTextureCubeMap::GlTextureCubeMap(unsigned int texId)
{
	id = texId;
	type = GL_TEXTURE_CUBE_MAP;
}

void GlTextureCubeMap::alloc(){
	allocated=true;
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT );
	checkGlError("GlTexture2D::alloc()");
}

void GlTextureCubeMap::dealloc(){
	glDeleteTextures(1,&id);
	allocated=false;
}
void GlTextureCubeMap::setup(GLint format,ivec2 texSize,GLenum datatype,GLenum face)
{
	if(!allocated){
		alloc();
	}
	size = texSize;
    glBindTexture(type, id);
    glTexImage2D(face,0,format,size.x,size.y,0,format,datatype,0);
	checkGlError("GlTexture2D::setup()");
    backed = true;
}
void GlTextureCubeMap::setImage(GLint format,ivec2 texSize,GLenum datatype,GLenum face,void* data)
{
	if(data != 0) {

		glBindTexture(type, id);
		checkGlError("glBindTexture");
		glTexSubImage2D(face,0,0,0,size.x,size.y,format,datatype,data);
		checkGlError("glTexSubImage2D");
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		//texture is NULL
	}
}

} // namespace gl
