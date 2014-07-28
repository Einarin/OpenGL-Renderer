#include <iostream>
#include "EigenSamples.h"
#include "catch.hpp"
#include "RK4Integrator.h"

#define CATCH_CONFIG_MAIN

using namespace Eigen;

//void simpleEigenTest(void)
//{
//	EigenSamples samples;
//	samples.SimpleProgram();
//	samples.RandomMatrix();
//}
/*
TEST_CASE( "RK4 constant acceleration", "[rk4constant]" ) {
	AccelerationFunction acceleration = []() { return 1; };
	RK4Integrator rk4(acceleration);


	REQUIRE(rk4.evaluate()
}


*/
Vector3d constantAcceleration(const State&, const float t)
{
	return Vector3d(1,0,0); 
}

void main()
{
	//Vector3d(const State&, const float t)
	AccelerationFunction accelerationFunction = [=](const State state, const float t) {
		return constantAcceleration(state, t);
	};
	RK4Integrator rk4(accelerationFunction);
	State state;
	Derivative d;

	state.x = Vector3d::Constant(0,0,0);
	state.v = Vector3d::Constant(0,0,0);

	//eval 3x, then integrate
}