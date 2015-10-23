#pragma once
#include "Geometry.h"
#include "Texture2.h"

namespace gl{

//This class provides an object that is drawn in screen space (ex: a portion of the HUD)
class ScreenDrawable {
protected:
	Billboard bb;
public:
	enum DrawableType {
		TEXTURE_DRAWABLE
	};
	ScreenDrawable();
	virtual ~ScreenDrawable();
	void setPosition(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 botLeft, glm::vec2 botRight);
	void setPosition(float x, float y, float width, float height);
	void setCenterPosition(float x, float y, float width, float height, float angle);
	void setCenterPosition(int x, int y, int width, int height);
	void setCenterPosition(float x, float y, float width, float height);
	DrawableType type();
	virtual void draw()=0;
};

class ScreenTexture : public ScreenDrawable {
protected:
	Tex2D texture;
public:
	ScreenTexture(Tex2D texture);
	virtual void draw();
};

} //namespace gl
