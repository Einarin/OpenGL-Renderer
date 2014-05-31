// Wrap around boost multiarray
#include "boost/multi_array.hpp"

template <typename ValueType>
class Grid {
public:
	Grid();
	Grid(int rows, int cols);

	virtual ~Grid();

	int numRows() const;
	int numCols() const;

	void resize(int nRows, int nCols);
private:
	boost::multi_array<ValueType, 2> *elements;
	int nRows;
	int nCols;
};

template <typename ValueType>
Grid<ValueType>::Grid()
{
	elements = NULL;
	nRows = 0;
	nCols = 0;
}

template <typename ValueType>
Grid<ValueType>::Grid(int rows, int cols)
{
	nRows = rows;
	nCols = cols;
	elements = new boost::multi_array<ValueType,2>(boost::extents[nRows][nCols]);
}

template <typename ValueType>
void Grid<ValueType>::resize(int rows, int cols)
{

}

