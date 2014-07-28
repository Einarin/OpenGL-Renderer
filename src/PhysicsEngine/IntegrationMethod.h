#include "Observables.h"
#include <functional>


class IntegrationMethod {
public:
	IntegrationMethod();
	IntegrationMethod(AccelerationFunction accelerationFunction)
	{
		acceleration = accelerationFunction;
	}
	virtual Derivative evaluate(const State& initial, float t, float dt, const Derivative &d) = 0;
	virtual void integrate(State& state, float t, float dt) = 0;

protected:
	AccelerationFunction acceleration;
};
