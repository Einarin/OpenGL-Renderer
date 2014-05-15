#pragma once
#include <vector>

namespace gl{

class VertexArrayObject{
protected:
	unsigned int vao;
	std::vector<unsigned int> ownedBuffers;
public:
	VertexArrayObject();
	~VertexArrayObject();
	void bind();
	void addAndBindBuffer(int target);
};

}