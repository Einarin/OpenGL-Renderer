
/// TODO this could wrapped with ProtoBuf/Blink/Capn
/// TODO support descriptor for Units (speed, mass, acceleration). If using boost, must check that it is serializable and can be sent "over the wire"

#include <boost/units/unit.hpp>
#include <boost/units/dimension.hpp>
#include <functional>

namespace bu = boost::units;

typedef std::function<float(const State&, const float t)> AccelerationFunction;

struct State {
	float x; // position
	float v; // velocity
};
 
struct Derivative
{
	float dx; // dx/dt = velocity
	float dv; // dv/dt = acceleration 
};