#ifndef MATHUTILITIES_INCLUDED
#define MATHUTILITIES_INCLUDED

#include <glm/glm.hpp>
#include <math.h>
//#include <VertexTypes.h>

#define PI 3.1415926535897932384626433832795

inline bool IsAboutEqual(float num1, float num2, float tolerance = 0.1f) {
	return abs(num1 - num2) <= tolerance;
}

inline bool IsAboutEqual(const glm::vec3& vector1, const glm::vec3& vector2, float tolerance = 0.1f) {
	return IsAboutEqual(vector1.x, vector2.x, tolerance) && IsAboutEqual(vector1.y, vector2.y, tolerance) && IsAboutEqual(vector1.z, vector2.z, tolerance);
}

#endif
