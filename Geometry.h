#pragma once
#include "glincludes.h"
namespace gl{

	class Geometry
    {
	public:
		virtual void init()=0;
        virtual void download()=0;
        virtual void draw()=0;
        virtual ~Geometry()
        {
        }
    };

	class Billboard : public Geometry{
	protected:
		unsigned int vao;
		unsigned int vbo;
		struct vertex{
			glm::vec4 pos;
			glm::vec4 txc;
		};
		vertex corners[4];
	public:
		Billboard();
		Billboard(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 botLeft, glm::vec3 botRight);
		Billboard(float topLeftX, float topLeftY, float width, float height);
		virtual void init();
		virtual void download();
		void move(float topLeftX, float topLeftY, float width, float height);
		void move(float topLeftX, float topLeftY);
		void moveRel(float dx, float dy);
		virtual void draw();
		virtual ~Billboard();
	};

}//namespace gl