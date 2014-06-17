// A vector field contains:
// An overloaded value operator that must return a VECTOR at a given POINT.
// An overloaded + operator
// An overloaded * operator

#include <list>
#include <Eigen/Dense>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;

template <class T>
class VectorField
{
public:
	VectorField();
	virtual VectorField& operator+(const VectorField &rhs);
	virtual VectorField& operator*(const VectorField&rhs);
	virtual Eigen::VectorXd operator() (bg::model::point<T, 3, bg::cartesian>) = 0;
};