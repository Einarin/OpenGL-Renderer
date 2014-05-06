#include "Quaternion.h"
#include <sstream>

Quaternion::Quaternion()
	: _axis(0, 0, 0), _w(1) { }

Quaternion::Quaternion(float w, float x, float y, float z)
	: _axis(x, y, z), _w(w) { }
	
Quaternion::Quaternion(float w, glm::vec3 axis)
	: _axis(axis), _w(w) { }

Quaternion::~Quaternion() { }

/**
 * @return Returns the magnitude of this Quaternion
 */
float Quaternion::GetMagnitude() {
	return sqrt(_axis.x * _axis.x +
				_axis.y * _axis.y +
				_axis.z * _axis.z +
				_w * _w);
}

/**
 * Normalizes this Quaternion (making its magnitude 1)
 */
void Quaternion::Normalize() {
	float magnitude = GetMagnitude();
	_axis /= magnitude;
	_w /= magnitude;
}

/**
 * @return Returns the conjugate of this quaternion.
 */
Quaternion Quaternion::GetConjugate() {
	return Quaternion(_w, -_axis.x, -_axis.y, -_axis.z);
}

/**
 * Calculates the product of the two quaternions.
 * @param a The first quaternion.
 * @param b The second quaternion
 * @param result The quaternion to store the result in.
 */
inline void GetProduct(const Quaternion& a, const Quaternion& b, Quaternion& result) {
	result = Quaternion(a.GetW() * b.GetW() - a.GetX() * b.GetX() - a.GetY() * b.GetY() - a.GetZ() * b.GetZ(),
						a.GetW() * b.GetX() + a.GetX() * b.GetW() + a.GetY() * b.GetZ() - a.GetZ() * b.GetY(),
					    a.GetW() * b.GetY() - a.GetX() * b.GetZ() + a.GetY() * b.GetW() + a.GetZ() * b.GetX(),
					    a.GetW() * b.GetZ() + a.GetX() * b.GetY() - a.GetY() * b.GetX() + a.GetZ() * b.GetW());
}

/**
 * Multiplies this quaternion with the other one.
 * @param other The other Quaternion to multiply against
 * @return Returns the product of the two quaternions as another quaternion.
 */
Quaternion Quaternion::operator*(const Quaternion& other) {
	Quaternion result;
	GetProduct(*this, other, result);
	return result;
}

/**
 * Multiplies this quaternion with the other one.
 * @param other The other Quaternion to multiply against
 * @return Returns the product of the two quaternions as another quaternion.
 */
const Quaternion& Quaternion::operator*=(const Quaternion& other) {
	GetProduct(*this, other, *this);
	return *this;
}

/**
 * Generates a rotation matrix from this quaternion.
 * @return Returns the matrix that was computed.
 */
glm::mat4 Quaternion::GetMatrix() {
	return glm::mat4(1 - 2 * (_axis.y * _axis.y + _axis.z * _axis.z), 2 * (_axis.x * _axis.y - _w * _axis.z),			2 * (_axis.x * _axis.z + _w * _axis.y),			 0,
					 2 * (_axis.x * _axis.y + _w * _axis.z),		  1 - 2 * (_axis.x * _axis.x + _axis.z * _axis.z),  2 * (_axis.y * _axis.z - _w * _axis.x),			 0,
					 2 * (_axis.x * _axis.z - _w * _axis.y),		  2 * (_axis.y * _axis.z + _w * _axis.x),			1 - 2 * (_axis.x * _axis.x + _axis.y * _axis.y), 0,
					 0,												  0,												0,												 1); 
}

/**
 * Rotates the given vector by this quaternion.
 * @param vectorToRotate The vector to rotate.
 * @return Returns a the given vector rotated by this quaternion.
 */
glm::vec3 Quaternion::RotateVector(glm::vec3 vectorToRotate) {
	Quaternion vectorAsQuaternion(0, vectorToRotate);
	Quaternion result = (*this) * vectorAsQuaternion * GetConjugate();
	return (1.0f / result.GetAxis().length()) * result.GetAxis();
}

/** 
 * @return Returns a string representation of this quaternion in the form: w, x, y, z
 */
std::string Quaternion::ToString() {
	std::stringstream stream;
	stream << _w << ", " << _axis.x << ", " << _axis.y << ", " << _axis.z << std::endl;
	return stream.str();
}

/**
 * Makes a quaternion that represents a rotation around the specified vector by
 *    the specified number of radians.
 * @param vector The vector to rotate around.
 * @param angle The angle to rotate, in radians.
 * @return Returns the created Quaternion.
 */
Quaternion Quaternion::RotateAround(glm::vec3 vector, float angle) {
	float w = (float)cos(angle / 2.0f);
	glm::vec3 xyzVector = (float)sin(angle / 2.0f) * vector;
	Quaternion result(w, xyzVector);
	result.Normalize();
	return result;
}

/**
 * Creates a quaternion that will rotate the given vector to another vector
 * @param from The vector to rotate.  Must be normalized.
 * @param to The vector you want to rotate [from] to.  Must be normalized.
 * @return Returns the created Quaternion.
 */
Quaternion Quaternion::RotateVectorToVector(glm::vec3 from, glm::vec3 to) {
	glm::vec3 rotationAxis = glm::normalize(glm::cross(to, from));
	float angle = acos(glm::clamp(glm::dot(from, to), -1.0f, 1.0f));
	return RotateAround(rotationAxis, angle);
}
