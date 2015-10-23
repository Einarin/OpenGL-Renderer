#pragma once
#include "glm/glm.hpp"
class Material
{
public:
	glm::vec3 materialColor;
	float metalness;
	float smoothness;
	glm::vec3 subsurfaceScatteringTransmission;
	glm::vec3 normal;
};