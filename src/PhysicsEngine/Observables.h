
/// TODO this could wrapped with ProtoBuf/Blink/Capn
/// TODO support descriptor for Units (speed, mass, acceleration). If using boost, must check that it is serializable and can be sent "over the wire"
/// TODO use Eigen to make all the quantities vectors
#include <Eigen/Dense>
#include <functional>

using namespace Eigen;

struct State;
struct Derivative;

typedef std::function<Vector3d(const State, const float)> AccelerationFunction;

struct State {
	Vector3d x; // position
	Vector3d v; // velocity
	float m; // mass
};
 
struct Derivative
{
	Vector3d dx; // dx/dt = velocity
	Vector3d dv; // dv/dt = acceleration 
	float dm; // dm/dt = change in mass
};
