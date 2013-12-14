#pragma once
#include "glincludes.h"
#include <vector>
#include "shader.h"
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

	class IndexedGeometry : public Geometry
    {
	protected:
		unsigned int vao;
		unsigned int vbo;
		unsigned int ibo;
		struct vertex{
			glm::vec4 pos;
			glm::vec4 tc;
			glm::vec4 normal;
		};
		std::vector<vertex> verts;
		std::vector<unsigned int> indices;
	public:
		virtual void init();
		virtual void download();
		virtual void draw();
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

	class StarBox : public Geometry {
	protected:
		unsigned int vao;
		unsigned int vbo;
		std::vector<glm::vec4> verts;
		Ref<Shader> starShader;
	public:
		StarBox();
		virtual void init();
		virtual void download();
		void setPos(glm::vec3 pos);
		virtual void draw();
	};

	class Sphere : public IndexedGeometry {
	protected:
		static void tesselate(std::vector<vertex>& verts,std::vector<unsigned int>& indices,unsigned int tesselationFactor,glm::vec2 start,glm::vec2 end);
	public:
		Sphere(unsigned int tesselationFactor);
		Sphere(unsigned int tesselationFactor,glm::vec2 start,glm::vec2 end);
	};

	class Cube : public IndexedGeometry {
		protected:
		static void tesselate(std::vector<vertex>& verts,std::vector<unsigned int>& indices,glm::ivec3 tesselationFactor);
	public:
		Cube(glm::ivec3 tesselationFactor);
	};

	class PatchSphere : public Sphere {
	protected:
		struct Patch{
			unsigned int vbo;
		unsigned int ibo;
		};
		unsigned int vao;
		std::vector<Patch> patches;
		glm::vec2 patchCount;
	public:
		void generate(glm::vec3 camerapos,glm::vec2 patchCount,unsigned int scaleDetail);
		virtual void init();
		virtual void download();
		virtual void draw();
	};

}//namespace gl