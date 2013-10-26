#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

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
		position += offset;
	}
	void Camera::rotate(float angle, glm::vec3 axis){
		::glm::rotate(q,angle,axis);
	}
	::glm::mat4 Camera::toMat4(){
		mat4 tmp;
		tmp = glm::translate(tmp,position);
		//tmp = glm::rotate(tmp,q);
		return tmp;
		
	}

} //namespace gl