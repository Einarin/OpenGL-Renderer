#include "TransformFeedback.h"
#include "glincludes.h"

namespace gl{

TransformFeedback::TransformFeedback(unsigned int primitiveType):m_primitiveType(primitiveType)
{
}

void TransformFeedback::init(){
	glGenBuffers(1, &m_buffer);
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glGenTransformFeedbacks(1, &m_feedbackObject);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_feedbackObject);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_buffer);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	} else {
		glGenQueries(1,&m_queryObject);
	}
	checkGlError("Transform Feedback Init");
}

void TransformFeedback::enable(){
	glEnable(GL_RASTERIZER_DISCARD);
	checkGlError("Disable Rasterizer");
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,m_feedbackObject);
	} else {
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_buffer);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_queryObject); 
	}
	glBeginTransformFeedback(m_primitiveType);
}

void TransformFeedback::disable(){
	glEndTransformFeedback();
	checkGlError("End Transform Feedback");
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	} else {
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		checkGlError("End Transform Feedback Query");
	}
	glDisable(GL_RASTERIZER_DISCARD);
	checkGlError("Enable Rasterizer");
}

bool TransformFeedback::pause(){
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glPauseTransformFeedback();
		return true;
	}
	return false;
}
bool TransformFeedback::resume(){
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,m_feedbackObject);
		glResumeTransformFeedback();
		return true;
	}
	return false;
}

unsigned int TransformFeedback::getBuffer(){
	return m_buffer;
}

void TransformFeedback::draw(){
	if(SupportFor(GL_TRANSFORM_FEEDBACK_2)){
		glDrawTransformFeedback(m_primitiveType,m_feedbackObject);
	} else {
		unsigned int primitivesWritten;
		glGetQueryObjectuiv(m_queryObject,GL_QUERY_RESULT, &primitivesWritten);
		glDrawArrays(m_primitiveType,0,primitivesWritten);
	}
}
void TransformFeedback::draw(int count){
	glDrawArrays(m_primitiveType,0,count);
}

TransformFeedback::~TransformFeedback(void)
{
}

} //namespace gl