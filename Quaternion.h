#pragma once

#include <glm/glm.hpp>
#include <string>

class Quaternion {
public:
	Quaternion();
	Quaternion(float w, float x, float y, float z);		
	Quaternion(float w, glm::vec3 axis);
	~Quaternion();

	float GetMagnitude();
	void Normalize();
	Quaternion GetConjugate();

	Quaternion operator*(const Quaternion& other);
	const Quaternion& operator*=(const Quaternion& other);

	inline float GetX() const { return _axis.x; }
	inline float GetY() const { return _axis.y; }
	inline float GetZ() const { return _axis.z; }
	inline float GetW() const { return _w; }
	inline glm::vec3 GetAxis() { return _axis; }

	glm::mat4 GetMatrix();
	glm::vec3 RotateVector(glm::vec3 vectorToRotate);

	std::string ToString();

	/// Factory methods

	static Quaternion RotateAround(glm::vec3 vector, float angle);
	static Quaternion RotateVectorToVector(glm::vec3 from, glm::vec3 to);

private:
	glm::vec3 _axis;
	float _w;
};

