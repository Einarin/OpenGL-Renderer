#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace gl {
	using namespace ::glm;
	Camera::Camera()
	{}
	Camera::Camera(vec3 eye, vec3 target, vec3 up):position(eye){
		Camera::lookAt(eye,target,up);
	}
	void Camera::lookAt(::glm::vec3 eye, ::glm::vec3 target, ::glm::vec3 up){
		mat4 tmp = ::glm::lookAt(position,target,up);
		q = quat_cast(tmp);
		position = tmp[3].xyz;
	}
	void Camera::moveTo(glm::vec3 place){
		position = place;
	}
	void Camera::moveBy(glm::vec3 offset){
		position += inverse(q) * offset;
	}
	void Camera::rotate(float angle, glm::vec3 axis){
		q = ::glm::rotate(q,angle,axis);
	}
	void Camera::rotate(::glm::quat rotation){
		q = rotation * q;
	}
	void Camera::zeroZ(){
		/*vec3 rotation = glm::eulerAngles(q);
		rotation.z = 0;*/
		glm::quat iq = inverse(q);
		vec3 zvec = iq * vec3(0.f,0.f,1.f);
		float angle = dot(vec3(0.f,1.f,0.f),iq * vec3(0.f,1.f,0.f));
		std::cout << angle << " " << acos(angle)*180.f/3.1415f << std::endl;
		q = glm::rotate(q,acos(angle),zvec);
	}
	::glm::mat4 Camera::toMat4(){
		mat4 tmp;
		tmp = glm::mat4_cast(q);
		tmp = glm::translate(tmp,position);
		return tmp;
		
	}
	::glm::vec3 Camera::pos(){
		return position;
	}

	::glm::vec3 Camera::front(){
		glm::quat iq = inverse(q);
		vec3 zvec = iq * vec3(0.f,0.f,1.f);
		return zvec;
	}

} //namespace gl