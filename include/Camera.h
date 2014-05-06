#pragma once

#include "glm/glm.hpp"
#include "Quaternion.h"

namespace gl {

/**
 * A Camera contains all the information about the 
 *    current view, provides methods for modifying the
 *    view and also provides GetViewMatrix and GetProjectionMatrix,
 *    which calculate and return matrices representing the properties
 *    that were set.
 */
class Camera {
public:
	Camera();
	virtual ~Camera();

	/// Methods to manipulate the Camera

	void MoveForward(float distance);
	void MoveBackward(float distance);
	void StrafeLeft(float distance);
	void StrafeRight(float distance);
	void MoveUp(float distance);
	void MoveDown(float distance);
	void RollClockwise(float angle);
	void RollCounterClockwise(float angle);
	void PitchUp(float angle);
	void PitchDown(float angle);
	void YawRight(float angle);
	void YawLeft(float angle);
	void ApplyRotation(Quaternion& rotation);
	void RotateAround(glm::vec3 vector, float angle);

	/// Properties

	// The position of the camera
	void SetPosition(glm::vec3 position);
	glm::vec3 GetPosition() { return _position; }

	// The Target is where the camera is looking
	void SetTarget(glm::vec3 target);
	glm::vec3 GetTarget();

	// A vector representing the direction the camera
	//    is pointing.
	glm::vec3 GetForwardVector();

	// A vector representing which direction is Up
	glm::vec3 GetUpVector();

	// A vector representing which direction is Right, 
	//    with respect to the UpVector and the ForwardVector
	glm::vec3 GetRightVector();

	// The view matrix, computed from the position, target and direction.
	glm::mat4 GetViewMatrix();

	// An angle (in degrees) representing the field of view of the camera.
	void SetFieldOfView(float fieldOfView);
	float GetFieldOfView() { return _fieldOfView; }

	// The aspect ratio of this camera
	void SetAspectRatio(float aspectRatio);
	float GetAspectRatio() { return _aspectRatio; }

	// The minimum distance from the camera in world coordinates
	//    that an element must be from the camera to be drawn
	void SetNearDistance(float nearDistance);
	float GetNearDistance() { return _nearDistance; }

	// Sets the maximum distance away from the camera an element
	//    can be in order to be drawn.
	void SetViewDistance(float viewDistance);
	float GetViewDistance() { return _viewDistance; }

	// The projection matrix
	glm::mat4 GetProjectionMatrix();

	void LogLocation();

protected:
	/// Protected members

	glm::vec3 _position;
	Quaternion _rotation;

	float _fieldOfView;
	float _aspectRatio;
	float _nearDistance;
	float _viewDistance;
};

}
