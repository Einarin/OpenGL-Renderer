#pragma once
#include "glincludes.h"
#include "Renderer.h"
#include "TransformFeedback.h"
#include "ThreadPool.h"
#include "AABB.h"
#include <list>
#include <memory>

namespace gl {
class AsteroidRenderer
{
protected:
	class Asteroid{
	public:
		glm::mat4 modelMatrix;
		AABB3 BoundingBox;
		TransformFeedback tfGeometry;
		bool generated;
		Asteroid():tfGeometry(GL_TRIANGLES),generated(false)
		{}
	};
	std::list<std::unique_ptr<Asteroid>> m_asteroids;
	std::shared_ptr<Shader> feedbackShader;
	std::shared_ptr<Shader> drawShader;
	bool m_setup;
public:
	AsteroidRenderer(void);
	~AsteroidRenderer(void);
	bool setup();
	inline int asteroidCount() {
		return m_asteroids.size();
	}
	void reset();
	Future<bool> addAsteroidAsync(glm::mat4 modelMatrix, glm::vec3 seed);
	virtual void draw(MvpShader s);
	void drawBoundingBoxes(MvpShader s);
};

}
