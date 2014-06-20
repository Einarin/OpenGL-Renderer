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
void main()
{
	AccelerationFunction acceleration = []() { return Vector3d::Constant(1,0,0); };
	RK4Integrator rk4(acceleration);
	State state;
	Derivative d;
	state.x = Vector3d::Constant(0,0,0);
	state.v = Vector3d::Constant(0,0,0);
	d.dv = Vector3d::Constant(0,0,0);
	d.dx = acceleration(&state, 1);
	//eval 3x, then integrate
}