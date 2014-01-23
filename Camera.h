#pragma once
#include "glincludes.h"
#include <glm/gtc/quaternion.hpp>

namespace gl {
	class Camera{
	protected:
		glm::quat rotation;
		glm::vec3 position;
	public:
		Camera();
		Camera(::glm::vec3 eye, ::glm::vec3 target, ::glm::vec3 up);
		void lookAt(::glm::vec3 eye, ::glm::vec3 target, ::glm::vec3 up);
		void moveTo(::glm::vec3 place);
		void moveBy(::glm::vec3 offset);
		void moveLocal(::glm::vec3 offset);
		void rotate(float angle, ::glm::vec3 axis);
		void rotate(::glm::quat rotation);
		void zeroZ();
		::glm::mat4 toMat4();
		glm::quat rot();
		::glm::vec3 pos();
		::glm::vec3 front();
	};

}