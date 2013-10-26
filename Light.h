#pragma once
#include <string>
#include "glincludes.h"

namespace gl{
	using glm::vec3;

	class Light{
	public:
		enum types {
			Undefined=0,
			Directional=1,
			Point=2,
			Spot=4
		};

		vec3 position;
		vec3 direction;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
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