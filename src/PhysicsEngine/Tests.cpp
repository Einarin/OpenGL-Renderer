#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"
#include <iostream>
#include "EigenSamples.h"
#include "RK4Integrator.h"

using namespace Eigen;

void simpleEigenTest(void)
{
	EigenSamples samples;
	samples.SimpleProgram();
	samples.RandomMatrix();
}

Vector3d constantAcceleration(const State&, const float t)
{
	return Vector3d(1,0,0); 
}

Derivative rk4Test_A()
{
	AccelerationFunction acceleration = [=](const State state, const float t) {
		return constantAcceleration(state, t);
	};
	RK4Integrator rk4(acceleration);

	State s;
	s.m = 0;
	s.v = Vector3d(1,0,0);
	s.x = Vector3d(1,0,0);

	Derivative d;
	d.dx = Vector3d(1,0,0);
	d.dv = Vector3d(0,0,0);
	d.dm = 0;

	return rk4.evaluate(s, 0, 1, d);
}

State rk4Test_B()
{
	AccelerationFunction acceleration = [=](const State state, const float t) {
		return constantAcceleration(state, t);
	};
	RK4Integrator rk4(acceleration);

	State s;
	s.m = 0;
	s.v = Vector3d(1,0,0);
	s.x = Vector3d(1,0,0);

	Derivative d;
	d.dx = Vector3d(1,0,0);
	d.dv = Vector3d(0,0,0);
	d.dm = 0;

	rk4.integrate(s, 0, 1);

	return s;
}

TEST_CASE( "rk4 constant acceleration", "[rk4constant]" ) {
	Derivative d_new = {Vector3d(1,0,0), Vector3d(1,0,0), 0};

	REQUIRE(rk4Test_A() == d_new);
}

TEST_CASE ("rk4 integration, constant acceleration", "[rk4integrate" ) {
	State s_new = {Vector3d(2.5,0,0), Vector3d(2,0,0), 0};
	State s_test = rk4Test_B();

	REQUIRE(s_test == s_new);
}