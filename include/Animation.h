#pragma once
#include <glm/glm.hpp>
#include <assimp/quaternion.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace gl {
	class Animation {
	public:
		struct Keyframe {
			double time;
			glm::vec3 position;
			aiQuaternion rotation; //We use aiQuaternions here so we can use assimp's spherical interpolation
			glm::vec3 scaling;
		};
		struct BoneAnimation {
			int boneIndex;
			std::vector<Keyframe> keyframes;
		};
		double length;
		std::string name;
		std::vector<BoneAnimation> bones;
	public:
		glm::mat4 getTransformAt(int boneId, double time) {
			double dt = fmod(time,length);
			int before = 0;
			int after = bones[boneId].keyframes.size() - 1;
			while (after - before > 1) {
				int pos = (after - before) / 2 + before;
				double val = bones[boneId].keyframes[pos].time;
				if (val > dt) {
					after = pos;
				} else {
					before = pos;
				}
			}
			Keyframe& beforeKey = bones[boneId].keyframes[before];
			Keyframe& afterKey = bones[boneId].keyframes[after];
			double interp = (dt - beforeKey.time) / (afterKey.time - beforeKey.time);
			glm::vec3 scaling = glm::mix(bones[boneId].keyframes[before].scaling, bones[boneId].keyframes[after].scaling, interp);
			//glm::quat rotation = glm::mix(bones[boneId].keyframes[before].rotation, bones[boneId].keyframes[after].rotation, interp);
			aiQuaternion aiRot;
			aiQuaternion::Interpolate(aiRot, bones[boneId].keyframes[before].rotation, bones[boneId].keyframes[after].rotation, interp);
			glm::quat rotation;
			#define QUAT_COPY(q1,q2) q1.w=q2.w;q1.x=q2.x;q1.y=q2.y;q1.z=q2.z
			QUAT_COPY(rotation, aiRot);
			#undef QUAT_COPY
			glm::vec3 translation = glm::mix(bones[boneId].keyframes[before].position, bones[boneId].keyframes[after].position, interp);
			glm::mat4 transformMat = glm::scale(glm::mat4(), scaling);
			transformMat = transformMat * glm::mat4_cast(rotation);
			transformMat = glm::translate(transformMat, translation);
			return transformMat;
		}
	};

}