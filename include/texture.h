#pragma once
#include "glincludes.h"
#include <string>
#include <memory>

namespace gl
{
class Texture {\
protected:
	bool allocated;
	unsigned int type;
public:
	Texture():allocated(false)
	{}
	virtual void init()=0;
	virtual void alloc()=0;
	virtual void bind()
	{}
	virtual void unbind()=0;
	virtual void dealloc()
	{}
	virtual void linearInterpolation()=0;
	virtual void nearestInterpolation()=0;
	virtual void wrap()=0;
	virtual void clamp()=0;
	virtual void mirror()=0;
	#ifdef _DEBUG
	//liable to be slow, intended for debugging only!
	virtual void draw()=0;
	#endif
	virtual ~Texture(){
		if(allocated){
			dealloc();
			allocated = false;
		}
	}
};

typedef std::shared_ptr<Texture> TexRef;

class TextureManager {
protected:
	TextureManager() {}
public:
	static TextureManager* Instance();
	virtual TexRef texFromFile(std::string image)=0;
	virtual TexRef texFromPngBytestream(char* buff, int bytes)=0;
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size)=0;
	virtual TexRef unbackedTex()=0;
	virtual TexRef backedTex(unsigned int format,glm::ivec2 size, unsigned int datatype)=0;
	virtual void contextLost()=0;
};

class GlTextureManager : public TextureManager {
public:
	virtual TexRef texFromFile(std::string image);
	virtual TexRef texFromPngBytestream(char* buff, int bytes);
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size);
	virtual TexRef unbackedTex();
	virtual TexRef backedTex(unsigned int format,glm::ivec2 size, unsigned int datatype);
	virtual void contextLost();
};

class GlTexture : public Texture {
protected:
	unsigned int id;
    bool backed;
	GLenum type;
	GLenum mapMode;
	GLenum interpolation;
	void setInterpolation();
	void setMapping();
public:
	GlTexture();
	virtual ~GlTexture();
	virtual void init();
	unsigned int getId();
	GLenum getType(){
		return type;
	}
	virtual void bind();
	virtual void unbind();
	virtual void linearInterpolation();
	virtual void nearestInterpolation();
	virtual void wrap();
	virtual void clamp();
	virtual void mirror();
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
	void setup(GLint format,glm::ivec2 size,GLenum datatype);
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,void* data);
#ifdef _DEBUG
	void draw();
#endif
	protected:
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
#ifdef _DEBUG
	void draw(){
	//TODO: unimplemented
	}
#endif
};

class FileBackedGlTexture2D : public GlTexture2D {
public:
	std::string filename;
	virtual void init();
	virtual void bind();
};

}