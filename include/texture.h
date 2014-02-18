#pragma once
#include "glincludes.h"
#include <string>

namespace gl
{
class TexRef;

class Texture {
	friend class TexRef;
protected:
	bool allocated;
	bool valid;
	unsigned int refcount;
	virtual void unreferenced()=0;
	inline void retain()
	{
		refcount++;
	}
	inline void release()
	{
		refcount--;
		unreferenced();
	}
public:
	Texture():refcount(1),allocated(false),valid(false)
	{}
	virtual void init()=0;
	virtual void alloc()=0;
	virtual void bind()
	{}
	virtual void unbind()=0;
	virtual void dealloc()
	{}
	void linearInterpolation();
	void nearestInterpolation();
	virtual ~Texture(){
		if(allocated){
			dealloc();
			allocated = false;
		}
	}
};

class TexRef {
	friend class GlTextureManager;
private:
	Texture* texture;
protected:
	TexRef(Texture* tex) : texture(tex)
	{
		tex->retain();
	}
public:
	TexRef(const TexRef& other)
	{
		other.texture->retain();
		texture = other.texture;
	}
	~TexRef()
	{
		texture->release();
	}
	const TexRef& operator=(const TexRef& other)
	{
		other.texture->retain();
		texture->release();
		texture = other.texture;
		return other;
	}
	const inline Texture* operator->() const
	{
		return texture;
	}
	inline Texture* operator->()
	{
		return texture;
	}
};

class TextureManager {
protected:
	TextureManager() {}
public:
	static TextureManager* Instance();
	virtual TexRef texFromFile(std::string image)=0;
	virtual TexRef texFromPngBytestream(char* buff, int bytes)=0;
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size)=0;
	virtual TexRef unbackedTex(glm::ivec2 size)=0;
	virtual void contextLost()=0;
};

class GlTextureManager : public TextureManager {
public:
	virtual TexRef texFromFile(std::string image);
	virtual TexRef texFromPngBytestream(char* buff, int bytes);
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size);
	virtual TexRef unbackedTex(glm::ivec2 size);
	virtual void contextLost();
};

class GlTexture : public Texture {
protected:
	unsigned int id;
	bool initialized;
    bool backed;
	GLenum type;
public:
	GlTexture();
	virtual ~GlTexture();
	virtual void init();
	unsigned int getId();
	virtual void bind();
	virtual void unbind();
	virtual void alloc()=0;
	virtual void dealloc()=0;
};

class GlTexture2D : public GlTexture {
public:
    GlTexture2D();
    GlTexture2D(unsigned int texId);
	
    /** for initialization that requires a valid OpenGL context
        * 
        * will be called before the first draw call, and will be called again
        * if context is lost and assets need to be redownloaded to the GPU
        */
	virtual void alloc();
	virtual void dealloc();
	virtual void unreferenced();
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,void* data);
protected:
	void setup(GLint format,glm::ivec2 size,GLenum datatype);
	glm::ivec2 size;
};

class GlTextureCubeMap : public GlTexture2D {
public:
    GlTextureCubeMap();
    GlTextureCubeMap(unsigned int texId);

    /** for initialization that requires a valid OpenGL context
        * 
        * will be called before the first draw call, and will be called again
        * if context is lost and assets need to be redownloaded to the GPU
        */
	virtual void alloc();
	virtual void dealloc();
	void setup(GLint format,glm::ivec2 texSize,GLenum datatype,GLenum face);
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,GLenum face,void* data);
};

class FileBackedGlTexture2D : public GlTexture2D {
public:
	std::string filename;
	virtual void bind();
};

}