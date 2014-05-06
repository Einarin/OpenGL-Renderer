#pragma once

namespace gl {
class Camera;

class Renderer{
public:
	virtual void draw(Camera* c)=0;
};

}
