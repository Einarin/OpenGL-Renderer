#pragma once
#include <vector>
#include <string>
#include "Shader.h"
#include "VertexAttribBuilder.h"

namespace gl{

class TransformFeedback
{
protected:
	unsigned int m_primitiveType;
	unsigned int m_buffer;
	unsigned int m_vao;
	//only used if we have GL_TRANSFORM_FEEDBACK_2
	unsigned int m_feedbackObject; 
	//only used if we don't have GL_TRANSFORM_FEEDBACK_2
	unsigned int m_queryObject;
public:
	TransformFeedback(unsigned int primitiveType);
	TransformFeedback(TransformFeedback&& o) {
		m_primitiveType = o.m_primitiveType;
		m_buffer = o.m_buffer;
		o.m_buffer = 0;
		m_vao = o.m_vao;
		o.m_vao = 0;
		m_feedbackObject = o.m_feedbackObject;
		o.m_feedbackObject = 0;
		m_queryObject = o.m_queryObject;
		o.m_queryObject = 0;
	}
	void init();
	void setPrimitiveType(unsigned int type) {
		m_primitiveType = type;
	}
	void enable();
	void disable();
	bool pause(); // requires GL_TRANSFORM_FEEDBACK_2
	bool resume(); // requires GL_TRANSFORM_FEEDBACK_2
	void allocateStorage(unsigned int buffersize);
	void setupVao(int numVertexAttribArrays,VertexAttribBuilder& b);
	void draw(); //uses transform feedback state to determine how many vertices to draw
	void draw(int count); //faster if GL_TRANSFORM_FEEDBACK_2 not supported
	unsigned int getBuffer();

	~TransformFeedback(void);
};

}