#pragma once
#include "glincludes.h"
#include "renderer.h"
#include "TransformFeedback.h"
#include "ThreadPool.h"
#include <vector>

namespace gl {
class AsteroidRenderer
{
protected:
	class Asteroid{
	public:
		glm::mat4 modelMatrix;
		TransformFeedback tfGeometry;
		bool generated;
		Asteroid():tfGeometry(GL_TRIANGLES),generated(false)
		{}
	};
	std::vector<Asteroid> m_asteroids;
	std::shared_ptr<Shader> feedbackShader;
	std::shared_ptr<Shader> drawShader;
	bool m_setup;
public:
	AsteroidRenderer(void);
	~AsteroidRenderer(void);
	bool setup();
	Future<bool> addAsteroidAsync(glm::mat4 modelMatrix, glm::vec3 seed);
	virtual void draw(MvpShader s);
};

}