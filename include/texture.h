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
	virtual TexRef unbackedTex(glm::vec2 size)=0;
	virtual void contextLost()=0;
};

class GlTextureManager : public TextureManager {
public:
	virtual TexRef texFromFile(std::string image);
	virtual TexRef unbackedTex(glm::vec2 size);
	virtual void contextLost();
};

class GlTexture2D : public Texture {
private:
    unsigned int id;
public:
    GlTexture2D();
    GlTexture2D(unsigned int texId);
	virtual ~GlTexture2D();

    /** for initialization that requires a valid OpenGL context
        * 
        * will be called before the first draw call, and will be called again
        * if context is lost and assets need to be redownloaded to the GPU
        */
    virtual void init();
	virtual void alloc();
	virtual void dealloc();
	virtual void bind();
	virtual void unbind();
	virtual void unreferenced();
    void setup(GLint format,glm::ivec2 size,GLenum datatype);
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,void* data);
	unsigned int getId();
protected:
	glm::ivec2 size;
	bool initialized;
    bool backed;
};

class FileBackedGlTexture2D : public GlTexture2D {
public:
	std::string filename;
	virtual void bind();
};

}