#include "Camera.h"
#include <math.h>
#include "GLM\gtc\matrix_transform.hpp"
#include <sstream>
#include "MathUtilities.h"

namespace gl {
/**
* Constructs a new camera with some defaults
*/
Camera::Camera() {
	// Initialize some default camera values
	_position = glm::vec3(0, 0, 0);
	_rotation = Quaternion(1, 0, 0, 0);

	// Initialize some default perspective values
	_fieldOfView = 45.0f;
	_aspectRatio = 4.0f / 3.0f;
	_nearDistance = 0.1f;
	_viewDistance = 100000;
}

/**
* Cleans up any resources used by this Camera
*/
Camera::~Camera() {
}

/**
* Moves the camera forward by the given distance.
* @param distance The distance to move forward.
*/
void Camera::MoveForward(float distance) {
	glm::vec3 difference = distance * GetForwardVector();
	_position += difference;
}

/**
* Moves the camera backward by the given distance.
* @param distance The distance to move backward.
*/
void Camera::MoveBackward(float distance) {
	MoveForward(-distance);
}

/**
* Strafes the camera left by the given distance (direction remains the same).
* @param distance The distance to strafe left.
*/
void Camera::StrafeLeft(float distance) {
	StrafeRight(-distance);
}

/**
* Strafes the camera right by the given distance (direction remains the same).
* @param distance The distance to strafe right.
*/
void Camera::StrafeRight(float distance) {
	glm::vec3 difference = distance * GetRightVector();
	_position += difference;
}

/**
* Moves the camera up by the given distance.
* @param distance The distance to move up.
*/
void Camera::MoveUp(float distance) {
	glm::vec3 difference = distance * GetUpVector();
	_position += difference;
}

/**
* Moves the camera down by the given distance.
* @param distance The distance to move down.
*/
void Camera::MoveDown(float distance) {
	MoveUp(-distance);
}

/**
* Roll the camera by the given angle.
* @param angle The angle to roll the camera in radians clockwise (when looking forward).
*/
void Camera::RollClockwise(float angle) {
	Quaternion roller = Quaternion::RotateAround(GetForwardVector(), angle);
	_rotation = roller * _rotation;
	_rotation.Normalize();
}

/**
* Roll the camera by the given angle.
* @param angle The angle to roll the camera in radians counter-clockwise (when looking forward).
*/
void Camera::RollCounterClockwise(float angle) {
	RollClockwise(-angle);
}

/**
* Pitches the camera up by the given angle.
* @param angle The angle to pitch in radians from the XZ plane.
*/
void Camera::PitchUp(float angle) {
	Quaternion pitcher = Quaternion::RotateAround(GetRightVector(), angle);
	_rotation = pitcher * _rotation;
	_rotation.Normalize();
}

/**
* Pitches the camera down by the given angle.
* @param angle The angle to pitch in radians from the XZ plane.
*/
void Camera::PitchDown(float angle) {
	PitchUp(-angle);
}

/**
* Yaws (spins around the Y axis) the camera to the right.
* @param angle The angle to yaw by in radians.
*/
void Camera::YawRight(float angle) {
	Quaternion yawer = Quaternion::RotateAround(GetUpVector(), -angle);
	_rotation = yawer * _rotation;
	_rotation.Normalize();
}

/**
* Yaws (spins around the Y axis) the camera to the left.
* @param angle The angle to yaw by in radians.
*/
void Camera::YawLeft(float angle) {
	YawRight(-angle);
}

/**
* Applies the given rotation to this camera's current rotation.
* @param rotation A quaternion representing the rotation to apply.
*/
void Camera::ApplyRotation(Quaternion& rotation) {
	_rotation = rotation * _rotation;
	_rotation.Normalize();
}

/**
* Rotates around the given vector by the given angle
* @param vector The vector to rotate around.
* @param angle The angle to rotate.
*/
void Camera::RotateAround(glm::vec3 vector, float angle) {
	_rotation = Quaternion::RotateAround(vector, angle) * _rotation;
	_rotation.Normalize();
}

/**
* Sets the position of this camera.
*    Note: Moves the target as well.
* @param position The position of the camera in world space.
*/
void Camera::SetPosition(glm::vec3 position) {
	_position = position;
}

/**
* Sets the target for this camera.
* @param target A point in world space for this camera to look at.
*/
void Camera::SetTarget(glm::vec3 target) {
	// Target cannot equal the position.  If it does, just return
	if (target == _position) {
		//Log::Error("Camera::SetTarget: The target cannot equal the position.");
		return;
	}

	// First point the vector towards the target
	glm::vec3 normalizedTargetVector = glm::normalize(target - _position);
	glm::vec3 oldForward = GetForwardVector();
	float angleToTarget = acos(glm::clamp(glm::dot(oldForward, normalizedTargetVector), -1.0f, 1.0f));
	glm::vec3 axisForRotation = glm::normalize(glm::cross(oldForward, normalizedTargetVector));
	Quaternion rotationToTarget = Quaternion::RotateAround(axisForRotation, angleToTarget);
	_rotation = rotationToTarget * _rotation;

	// Now roll 0 degrees to the XZ plane
	glm::vec3 forward = GetForwardVector();
	glm::vec3 right = GetRightVector();
	glm::vec3 up = GetUpVector();

	glm::vec3 rightUpPlaneNormal = glm::cross(up, right);
	glm::vec3 intersectionVector;

	// Plane: 
	//    normal.x * x + normal.y * y + normal.z * z = 0 
	if (rightUpPlaneNormal.z != 0) {
		// z = -(normal.x * x + normal.y * y) / normal.z
		//		Intersects the x/z plane when y = 0
		//		So the line is z = -normal.x * x / (normal.z * z)
		//		Use the right vector for orientation
		float zIntersection = -rightUpPlaneNormal.x * right.x / rightUpPlaneNormal.z;
		intersectionVector = glm::normalize(glm::vec3(right.x, 0, zIntersection));
	} else if (right.z >= 0) {
		// If normal.z == 0, we are locked around that axis so the intersect is the +z axis itself
		//    Make the direction match the right vector's direction.
		intersectionVector = glm::vec3(0, 0, 1);
	} else {
		intersectionVector = glm::vec3(0, 0, -1);
	}

	// Get the angle between the intersection of the plane
	//    and the current right vector
	float dotProduct = glm::clamp(glm::dot(right, intersectionVector), -1.0f, 1.0f);
	float angle = acos(dotProduct);

	// Make a quaternion to rotate around forward by the angle	
	Quaternion fixRoll;
	fixRoll = Quaternion::RotateAround(glm::normalize(glm::cross(right, intersectionVector)), angle);
	_rotation = fixRoll * _rotation;

	// We can go upside down depending on which side of
	//    the intersection line we rolled to.
	//    Just flip us if we did
	glm::vec3 newUpVector = GetUpVector();
	if (newUpVector.y < 0)
		_rotation = Quaternion::RotateAround(GetForwardVector(), PI) * _rotation;
}

/**
* @return Returns the target of the camera.
*/
glm::vec3 Camera::GetTarget() {
	return _position + GetForwardVector();
}

/** 
*  @return Returns the forward vector for this camera.
*/
glm::vec3 Camera::GetForwardVector() {
	return glm::normalize(_rotation.RotateVector(glm::vec3(1, 0, 0)));
}

/**
* @return Returns the up vector for this camera.
*/
glm::vec3 Camera::GetUpVector() {
	return glm::normalize(_rotation.RotateVector(glm::vec3(0, 1, 0)));
}

/**
* @return Returns the right vector for this camera.
*/
glm::vec3 Camera::GetRightVector() {
	return glm::normalize(_rotation.RotateVector(glm::vec3(0, 0, 1)));
}

/**
* @returns Returns the view matrix.
*/
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(_position, GetTarget(), GetUpVector());
}

/** 
* Sets the camera's field of view.
* @param fieldOfView The field of view to set, in degrees.
*/
void Camera::SetFieldOfView(float fieldOfView) {
	_fieldOfView = fieldOfView;
}

/**
* Sets the aspect ratio for this camera.
* @param aspectRatio The aspect ratio for this camera.
*/
void Camera::SetAspectRatio(float aspectRatio) {
	_aspectRatio = aspectRatio;
}

/**
* Sets the near distance for this camera.
* @param nearDistance The minimum distance, in world coordinates,  the 
*                     camera away from for an element to be drawn.
*/
void Camera::SetNearDistance(float nearDistance) {
	_nearDistance = nearDistance;
}

/**
* Sets the view distance for this camera.
* @param viewDistance The maximum distance away from the camera
*                     an element can be in order to be drawn.
*/
void Camera::SetViewDistance(float viewDistance) {
	_viewDistance = viewDistance;
}

/**
* @returns Returns the Projection Matrix, recalculating if anything
*          it depends on has changed since last time it was called.
*/
glm::mat4 Camera::GetProjectionMatrix() {
	return glm::perspective(_fieldOfView, _aspectRatio, _nearDistance, _viewDistance);
}

/**
* Logs the position and angle of the camera
*/
void Camera::LogLocation() {
	std::stringstream stream;
	stream << "Camera Location:" << std::endl;
	stream << "\t\tRotation: " << _rotation.ToString();
	stream << "\t\tPosition: " << _position.x << ", " << _position.y << ", " << _position.z << std::endl;
	//Log::Message(stream.str());
}
} //namespace gl
