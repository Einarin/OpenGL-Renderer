#include "IntegrationMethod.h"

class RK4Integrator: public IntegrationMethod {
public: 
	RK4Integrator(AccelerationFunction& accelerationFunction) : IntegrationMethod()
	{
		acceleration = accelerationFunction;
	}

	Derivative evaluate(const State& initial, float t, float dt, const Derivative &d)
	{
		// New state position and velocity
		State state;
		state.x = initial.x + dt * d.dx;
		state.v = initial.v + dt * d.dv;

		// New derivative
		Derivative d_new;
		d_new.dx = state.v;
		d_new.dv = acceleration(&state, t + dt);

		return d_new;
	}

	void integrate(State& state, float t, float dt)
	{
		Derivative a,b,c,d;

		a = evaluate(state, t, 0.0f, Derivative() );
		b = evaluate(state, t, dt*0.5f, a);
		c = evaluate(state, t, dt*0.5f, b);
		d = evaluate(state, t, dt, c);

		float dxdt = 1.0f / 6.0f * (a.dx + 2.0f*(b.dx + c.dx) + d.dx);
		float dvdt = 1.0f / 6.0f * (a.dv + 2.0f*(b.dv + c.dv) + d.dv);

		state.x = state.x + dxdt * dt;
		state.v = state.v + dvdt * dt;
	}
};