#pragma once
#include "glincludes.h"
#include <string>
#include <memory>

namespace gl
{
#ifndef _DEBUG
#define TEXTURE_BINDING_DEBUG_CHECK 
#else
	void textureBindingDebugCheck(GLenum target, GLuint id,const char* file, int line);
#define TEXTURE_BINDING_DEBUG_CHECK textureBindingDebugCheck(getType(), m_id, __FILE__, __LINE__);
	/*{\
GLint id;\
glGetIntegerv(getTarget(), &id);\
if(id != m_id) {\
	std::cout << "TEXTURE NOT BOUND IN " << __FILE__ << ":" << __LINE__ << "\n";\
	onMismatch();\
}\
}*/
#endif
	template<GLenum target>
	class Texture {
	protected:
		GLuint m_id;
		Texture(GLuint id):m_id(id)
		{ }
		Texture():m_id(0){ }
	public:
		const GLenum getType() const { return target; }
		const GLuint getId() const{
			return m_id;
		}
		void init() {
			if (m_id == 0) {
				glGenTextures(1, &m_id);
			}
		}
		void setMagFilter(GLint interpolation){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, interpolation);
		}
		void setMinFilter(GLint interpolation){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, interpolation);
		}
		void setLinearFiltering(bool hasMipMaps){
			TEXTURE_BINDING_DEBUG_CHECK
			if (hasMipMaps){
				setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
				setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
			} else {
				setMagFilter(GL_LINEAR);
				setMinFilter(GL_LINEAR);
			}
		}
		void setWrapping(GLint wrapMode){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
		}
		void setClampEdges(){
			TEXTURE_BINDING_DEBUG_CHECK
			setWrapping(GL_CLAMP);
		}
		void setWrapEdges(){
			TEXTURE_BINDING_DEBUG_CHECK
			setWrapping(GL_REPEAT);
		}
		void bind(){
			glBindTexture(target, m_id);
		}
		void genMipMaps(){
			TEXTURE_BINDING_DEBUG_CHECK
			glGenerateMipmap(target);
		}
		void setNoMipMaps() {
			TEXTURE_BINDING_DEBUG_CHECK
			glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
		}
		~Texture(){
			if (m_id != 0){
				glDeleteTextures(1, &m_id);
				m_id = 0;
			}
		}

	};

	class Texture2D : public Texture < GL_TEXTURE_2D > {
	public:
		Texture2D(GLuint id) :Texture(id){ }
		Texture2D() : Texture() {}
		///Texture must be bound first
		void alloc(GLint format, GLint internalFormat, glm::ivec2 texSize, GLenum datatype){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texSize.x, texSize.y, 0, format, datatype, 0);
		}
		void setImage(GLint format, GLint mipLevel, glm::ivec2 texSize, GLenum datatype,const void* data){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, texSize.x, texSize.y, format, datatype, data);
		}
		///Sets mip level 0 with unsigned byte data
		void setImage(GLint format, glm::ivec2 texSize,const GLubyte* data){
			TEXTURE_BINDING_DEBUG_CHECK
			setImage(format, 0, texSize, GL_UNSIGNED_BYTE, (const void*)data);
		}
		void draw();
		static std::shared_ptr<Texture2D> backedTexture(GLint format, GLint internalFormat, glm::ivec2 texSize, GLenum datatype) {
			auto tex = new Texture2D();
			tex->init();
			tex->bind();
			tex->setClampEdges();
			tex->setLinearFiltering(false);
			tex->setNoMipMaps();
			tex->alloc(format, internalFormat, texSize, datatype);
			return std::shared_ptr<Texture2D>(tex);
		}
	};
	typedef std::shared_ptr<Texture2D> Tex2D;

	static const GLenum TextureCubeMapFaces[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	class TextureCubeMap : public Texture < GL_TEXTURE_CUBE_MAP > {
	public:
		
		static void makeSeamless(){
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		}
		void alloc(GLint format, GLint internalFormat, glm::ivec2 faceSize, GLenum datatype){
			TEXTURE_BINDING_DEBUG_CHECK
			for (GLenum face : TextureCubeMapFaces){
				glTexImage2D(face, 0, internalFormat, faceSize.x, faceSize.y, 0, format, datatype, 0);
			}
		}
		void allocFace(GLenum face, GLint format, GLint internalFormat, glm::ivec2 faceSize, GLenum datatype){
			TEXTURE_BINDING_DEBUG_CHECK
				glTexImage2D(face, 0, internalFormat, faceSize.x, faceSize.y, 0, format, datatype, 0);
		}
		void setFaceImage(GLenum face, GLint format, glm::ivec2 texSize, GLenum datatype,const void* data){
			TEXTURE_BINDING_DEBUG_CHECK
			glTexSubImage2D(face, 0, 0, 0, texSize.x, texSize.y, format, datatype, data);
		}
	};
	typedef std::shared_ptr<TextureCubeMap> TexCube;
#undef TEXTURE_BINDING_DEBUG_CHECK
}