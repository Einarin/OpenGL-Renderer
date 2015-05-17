#include "Primitives.h"
#include <cmath>

double PI = 3.14159265358979323;

std::valarray<glm::vec2> circlePos(int numPoints) {
	std::valarray<glm::vec2> points(numPoints + 1);
	points[0] = glm::vec2(0.f, 0.f);
	for (int i = 0; i < numPoints; i++) {
		float angle = (2.0f*PI*i) / float(numPoints);
		points[i+1] = glm::vec2(sin(angle), cos(angle));
	}
	return points;
}

std::valarray<int> circleIndex(int numPoints) {
	std::valarray<int> inds(3 * numPoints);
	for (int i = 0; i < numPoints-1; i++) {
		inds[3 * i] = 0;
		inds[(3 * i) + 1] = i+1;
		inds[(3 * i) + 2] = i+2;
	}
	inds[3 * (numPoints - 1)] = 0;
	inds[3 * (numPoints - 1)+1] = numPoints+1;
	inds[3 * (numPoints - 1) + 2] = 1;
	return inds;
}

std::pair<std::valarray<glm::vec2>, std::valarray<int>> circle(int numPoints) {
	return make_pair(circlePos(numPoints), circleIndex(numPoints));
}

std::pair<std::valarray<glm::vec3>, std::valarray<int>> cylinderPoints(int numRadial, int numAxial, bool endCaps) {
	auto circ = circlePos(numRadial);
	int size = numRadial*numAxial;
	if (endCaps) size += 2;
	std::valarray<glm::vec3> points(size);
	for (int zi = 0; zi < numAxial; zi++) {
		float z = (2.0f*zi) - 1.0f;
		for (int i = 0; i < numRadial; i++) {
			points[zi*numRadial + i] = glm::vec3(circ[i + 1], z);
		}
	}
	return make_pair(points,std::valarray<int>());
}