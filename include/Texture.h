#pragma once
#include "glincludes.h"
#include <string>
#include <memory>

namespace gl
{

	class GlTexture {
	protected:
		bool allocated;
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
		virtual void alloc() = 0;
		virtual void dealloc() = 0;
	};

typedef std::shared_ptr<GlTexture> TexRef;

class TextureManager {
protected:
	TextureManager() {}
public:
	static TextureManager* Instance();
	virtual TexRef texFromFile(std::string image, bool sRGB = true) = 0;
	virtual TexRef texFromPngBytestream(char* buff, int bytes, bool sRGB = true) = 0;
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size, bool sRGB = true) = 0;
	virtual TexRef unbackedTex()=0;
	virtual TexRef backedTex(unsigned int format,glm::ivec2 size, unsigned int datatype)=0;
	virtual TexRef backedTex(unsigned int format, glm::ivec2 size, unsigned int datatype, unsigned int internalFormat) = 0;
	virtual TexRef depthTex(unsigned int format,glm::ivec2 size, unsigned int datatype)=0;
	virtual TexRef missingTex()=0;
	virtual void contextLost()=0;
};

class GlTextureManager : public TextureManager {
public:
	virtual TexRef texFromFile(std::string image, bool sRGB = true);
	virtual TexRef texFromPngBytestream(char* buff, int bytes, bool sRGB = true);
	virtual TexRef texFromRGBA8888(char* buff, glm::ivec2 size, bool sRGB = true);
	virtual TexRef unbackedTex();
	virtual TexRef backedTex(unsigned int format, glm::ivec2 size, unsigned int datatype);
	virtual TexRef backedTex(unsigned int format, glm::ivec2 size, unsigned int datatype, unsigned int internalFormat);
	virtual TexRef depthTex(unsigned int format,glm::ivec2 size, unsigned int datatype);
	virtual TexRef missingTex();
	virtual void contextLost();
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
	void setup(GLint format, glm::ivec2 size, GLenum datatype, unsigned int internalFormat);
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,void* data);
	void draw();
	protected:
	glm::ivec2 size;
};

class GlTexture2DDepth : public GlTexture2D {
	public:
		void draw();
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
	void setInterpolation();
	void setMapping();
	void setup(GLint format,glm::ivec2 texSize,GLenum datatype,GLenum face);
	void setup(GLint format, glm::ivec2 texSize, GLenum datatype, GLenum face,GLint internalFormat);
	void setImage(GLint format,glm::ivec2 size,GLenum datatype,GLenum face,void* data);
	void draw(){
	//TODO: unimplemented
	}
};

class FileBackedGlTexture2D : public GlTexture2D {
public:
	std::string filename;
	bool sRGB;
	virtual void init();
	virtual void bind();
};

}
