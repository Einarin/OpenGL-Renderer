#pragma once
#include <vector>

namespace gl {
enum AttribDataTypes{
	FLOAT_ATTRIB,
	PADDING_ATTRIB
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
	//add attributes of type {FLOAT_ATTRIB,PADDING_ATTRIB}
	VertexAttribBuilder& attrib(AttribDataTypes type, int count);
	//add padding for memory in the buffer not used by any attributes
	VertexAttribBuilder& pad(int bytes);
	//set the vertex size explicitly. If this isn't called the size is calculated automatically
	VertexAttribBuilder& setSize(unsigned int size);
	//makes all the actual OpenGL calls. Doesn't modify the object so it can be called repeatedly
	void build() const;
	//returns the auto-calculated vertex size
	unsigned int getAutoSize(){
		return _size;
	}
	~VertexAttribBuilder(void);
};

}