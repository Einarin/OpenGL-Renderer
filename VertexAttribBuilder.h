#pragma once
#include <vector>

namespace gl {
enum AttribDataTypes{
	FLOAT_ATTRIB
};

class VertexAttribBuilder
{
private:
	struct Attrib{
		int count;
		unsigned int type;
		unsigned int size;
	};
	bool _manualSize;
	unsigned int _size;
	std::vector<Attrib> _attribs;
public:
	VertexAttribBuilder(void);
	VertexAttribBuilder& attrib(AttribDataTypes type, int count);
	VertexAttribBuilder& setSize(unsigned int size);
	void build();
	~VertexAttribBuilder(void);
};

}