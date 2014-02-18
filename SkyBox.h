#pragma once
#include "glincludes.h"
#include "Geometry.h"
#include "Texture.h"
#include "Camera.h"

namespace gl{

class SkyBox
{
protected:
	bool initialized,downloaded;
	std::shared_ptr<Shader> shader;
	GlTextureCubeMap cubemap;
	unsigned int texloc;
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	Cube cube;
public:
	SkyBox(void);
	~SkyBox(void);
	void init();
	void download();
	void setImage(std::string basepngfilename);
	void draw(Camera* c);
};

}; //namespace gl