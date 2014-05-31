#ifndef EIGENSAMPLES_INCLUDED   
#define EIGENSAMPLES_INCLUDED  

#include <iostream>
#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace std;

class EigenSamples
{
public:
	void SimpleProgram()
	{
		MatrixXd m(2,2);
		m(0,0) = 3;
		m(1,0) = 2.5;
		m(0,1) = -1;
		m(1,1) = m(1,0) + m(0,1);
		std::cout << m << std::endl;
	}
	void RandomMatrix()
	{
		MatrixXd m = MatrixXd::Random(3,3);
		m = (m + MatrixXd::Constant(3,3,1.2)) * 50;
		cout << " =" << endl << m << endl;

		VectorXd v(3);
		v << 1, 2, 3;
		cout << "m * v = " << endl << m * v << endl;
	}
};

#endif