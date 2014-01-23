#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

namespace gl {
	using namespace ::glm;
	Camera::Camera()
	{}
	Camera::Camera(vec3 eye, vec3 target, vec3 up){
		Camera::lookAt(eye,target,up);
	}
	void Camera::lookAt(::glm::vec3 eye, ::glm::vec3 target, ::glm::vec3 up){
		mat4 tmp = glm::lookAt(eye,target,up);
		rotation = quat_cast(tmp);
		position = tmp[3].xyz;
	}
	void Camera::moveTo(glm::vec3 place){
		position = place;
	}
	void Camera::moveBy(glm::vec3 offset){
		position += offset;
	}
	void Camera::moveLocal(glm::vec3 offset){
		vec4 change = rotation * vec4(offset.x,offset.y,offset.z,0.0f);
		position += vec3(change.xyz);
	}
	void Camera::rotate(float angle, glm::vec3 axis){
		rotation = glm::rotate(rotation,angle,axis);
	}
	void Camera::rotate(::glm::quat rotation){
		//data = data * rotation;
	}
	void Camera::zeroZ(){
		/*vec3 rotation = glm::eulerAngles(q);
		rotation.z = 0;*/
		glm::quat iq;// = inverse(q);
		vec3 zvec = iq * vec3(0.f,0.f,1.f);
		float angle = dot(vec3(0.f,1.f,0.f),iq * vec3(0.f,1.f,0.f));
		//std::cout << angle << " " << acos(angle)*180.f/3.1415f << std::endl;
		//q = glm::rotate(q,acos(angle),zvec);
	}
	::glm::mat4 Camera::toMat4(){
		/*rotation[0][3] = 0.f;
		rotation[1][3] = 0.f;
		rotation[2][3] = 0.f;*/
		return translate(glm::mat4_cast(rotation),position);
		
	}
	::glm::vec3 Camera::pos(){
		return position;
	}
	glm::quat Camera::rot(){
		return rotation;
	}

	::glm::vec3 Camera::front(){
		//glm::quat iq = inverse(q);
		//vec3 zvec = iq * vec3(0.f,0.f,1.f);
		vec3 forward(0.f,0.f,1.0f);
		return rotation * forward;
	}

} //namespace gl