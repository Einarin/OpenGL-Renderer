#include "FlatDrawable.h"
#include "glm/gtx/rotate_vector.hpp"

namespace gl {

	ScreenDrawable::ScreenDrawable()
	{}

	ScreenDrawable::~ScreenDrawable()
	{ }

	void ScreenDrawable::setPosition(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 botLeft, glm::vec2 botRight){
		bb.position(topLeft, topRight, botLeft, botRight);
	}

	void ScreenDrawable::setPosition(float x, float y, float width, float height)
	{
		bb.move(x,y,width,height);
	}
	void ScreenDrawable::setCenterPosition(float x, float y, float width, float height, float angle)
	{
		using glm::vec2;
		float topLeftX = width*0.5f;
		float topLeftY = width*0.5f;
		vec2 center(x,y);
		vec2 topLeft = glm::rotate(vec2(topLeftX,topLeftY),angle);
		vec2 topRight = glm::rotate(vec2(topLeftX+width,topLeftY),angle);
		vec2 botLeft = glm::rotate(vec2(topLeftX,topLeftY+height),angle);
		vec2 botRight = glm::rotate(vec2(topLeftX+width,topLeftY+height),angle);
		topLeft += center;
		topRight += center;
		botLeft += center;
		botRight += center;
		bb.position(topLeft,topRight,botLeft,botRight);
	}
	void ScreenDrawable::setCenterPosition(int x, int y, int width, int height)
	{
		float topLeftX = static_cast<float>(x - width/2);
		float topLeftY = static_cast<float>(y - width/2);
		bb.move(topLeftX,topLeftY,static_cast<float>(width),static_cast<float>(height));
	}
	void ScreenDrawable::setCenterPosition(float x, float y, float width, float height)
	{
		float topLeftX = x - width*0.5f;
		float topLeftY = y - width*0.5f;
		bb.move(topLeftX,topLeftY,width,height);
	}
	ScreenDrawable::DrawableType ScreenDrawable::type(){
		return TEXTURE_DRAWABLE; //All drawables are texture drawables by default
	}

	ScreenTexture::ScreenTexture(std::string resource):texture(TextureManager::Instance()->texFromFile(resource)){
	}

	ScreenTexture::ScreenTexture(TexRef tex):texture(tex){
	}

	void ScreenTexture::draw(){
		//TODO: finish
		bb.draw();
	}
}