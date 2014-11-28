#pragma once
#include <vector>
#include <utility>
#include "glincludes.h"

namespace gl{

class VertexArrayObject{
protected:
	unsigned int vao;
	std::vector<std::pair<GLenum,unsigned int>> ownedBuffers;
public:
	VertexArrayObject();
	~VertexArrayObject();
	void bind();
	/// Adds a new buffer to this VAO of type \param:target
	///  and returns the index at which it can be accessed
	unsigned int addAndBindBuffer(int target);
	/// Binds the buffer at the given VAO index
	///		note: NOT the OpenGL index
	void bindBuffer(int index);
};

}