#include "Observables.h"
#include <functional>

class IntegrationMethod {
public:
	IntegrationMethod();
	IntegrationMethod(AccelerationFunction& accelerationFunction)
	{
		acceleration = accelerationFunction;
	}
	Derivative evaluate(const State& initial, float t, float dt, const Derivative &d);
	virtual void integrate(State& state, float t, float dt);

protected:
	AccelerationFunction acceleration;
};