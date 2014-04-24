#pragma once
#include <string>
#include "glincludes.h"
#include "glm/glm.hpp"

namespace gl{

	class Light{
	public:
		enum types {
			Undefined=0,
			Directional=1,
			Point=2,
			Spot=4
		};

		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float attenuationConstant;
		float attenuationLinear;
		float attenuationQuadratic;
		float innerAngle;
		float outerAngle;
		Light::types type;
		glm::mat4 transform;
		std::string name;

		
	};
}