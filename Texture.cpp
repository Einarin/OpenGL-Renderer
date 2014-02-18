#pragma once
#include "Texture.h"
#include "ImageLoader.h"
#include <iostream>

namespace gl {

using glm::ivec2;

GlTextureManager* manager = NULL;

void Texture::linearInterpolation()
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::nearestInterpolation()
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

GlTexture::GlTexture():id(0),initialized(false),backed(false)
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
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
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
	initialized = true;
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
	//TODO: make threadsafe
	if(manager == NULL){
		manager = new GlTextureManager();
	}
	return manager;
}

void GlTexture2D::unreferenced(){
	//TODO: something?
}

TexRef GlTextureManager::texFromFile(std::string file){
	FileBackedGlTexture2D* tex = new FileBackedGlTexture2D();
	tex->filename = file;
	return TexRef(tex);
}
TexRef GlTextureManager::unbackedTex(glm::ivec2 size){
	return TexRef(new GlTexture2D());
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
void FileBackedGlTexture2D::bind(){
	if(!backed){
		if(!loadPngToGlTex2D(filename,(GlTexture2D*)this))
			std::cout << "Loading " << filename << " failed\n";
		checkGlError("loadPngToGlTex2D");
	}
	
	GlTexture2D::bind();
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
