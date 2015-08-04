#pragma once
#include "glincludes.h"
#include "Geometry.h"
#include "Texture2.h"
#include "Renderer.h"
#include "Camera.h"


namespace gl{

class SkyBox : public Renderer
{
protected:
	bool initialized,downloaded;
	std::shared_ptr<Shader> shader;
	TextureCubeMap cubemap;
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
	void setImageAsync(std::string basepngfilename);
	void draw(Camera* c);
};

}; //namespace gl