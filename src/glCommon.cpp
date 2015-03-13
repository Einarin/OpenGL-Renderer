#include "glincludes.h"
#include <string>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;

int checkGlErrorImpl(std::string step,std::string file,int line)
{
	int err = glGetError();
	if(err != 0)
	{
		std::string error = "unknown error";
		if(err == 1280) error = "GL_INVALID_ENUM";      
		if(err == 1281) error = "GL_INVALID_VALUE";     
		if(err == 1282) error = "GL_INVALID_OPERATION"; 
		if(err == 1283) error = "GL_STACK_OVERFLOW";    
		if(err == 1284) error = "GL_STACK_UNDERFLOW";   
		if(err == 1285) error = "GL_OUT_OF_MEMORY";
		if(err == 1286) error = "GL_INVALID_FRAMEBUFFER_OPERATION";
		cout << file << ":"<< line <<" GL error "<< error << " at " << step << endl;
		//printGlError(("GL error "+ error + " in step " +step + " at " + file + ":%d\r\n").c_str());
		return err;
	}
	else
	{
		//printGlDebug((step+" complete").c_str());
	}
	return 0;
}

bool gl_features[GL_FEATURES_COUNT];
int major_version;
int minor_version;
std::string gl_extensions;

void APIENTRY OpenglErrorCallback(GLenum source,
						 GLenum type,
						 GLuint id,
						 GLenum severity,
						 GLsizei length,
						 const GLchar* message,
						 const void* userParam)
{
	bool interrupt = false;
	std::string text;
	switch(severity){
	case GL_DEBUG_SEVERITY_HIGH:
		text = "High Severity ";
		interrupt = true;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		text = "Medium Severity ";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		text = "Low Severity ";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		text = "Notification ";
		break;
	}
	switch(type){
	case GL_DEBUG_TYPE_ERROR:
		text += "Error: ";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		text += "Deprecated: ";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		text += "Undefined Behavior: ";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		text += "Portability Warning: ";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		text += "Performance Warning: ";
		break;
	case GL_DEBUG_TYPE_MARKER:
		text += "Marker: ";
		break;
	case GL_DEBUG_TYPE_OTHER:
	default:
		//return;
		text += "Unknown: ";
		break;
	}
	cout << text << message << endl;
	switch(source){
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		interrupt = true;
		
		break;
	default:
		break;
	}
	if(interrupt){
		#ifdef _WIN32
			DebugBreak();
		#endif
	}
}

static unsigned int sFreeTexMemNum;

int FreeGpuMemoryMB(){
	int mem[] = {0,0,0,0};
	if (sFreeTexMemNum){
		glGetIntegerv(sFreeTexMemNum, mem);
	}
	return mem[0];
}

bool SetupSupport(){
	bool status = true;
	std::string version((const char*)glGetString(GL_VERSION));
	
	int spaceEnd = version.find_last_of(' ');
	int dotEnd = version.find_last_of('.');
	major_version = version[spaceEnd+1] - 0x30; //convert ASCII to number
	minor_version = version[dotEnd+1] - 0x30;

	cout << "OpenGL version " << major_version << '.' << minor_version << " supported" << endl;
	cout << "Vendor is " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer is " << glGetString(GL_RENDERER) << endl;

	const char* ptr = (const char*)glGetString(GL_EXTENSIONS);
	if(ptr != nullptr){
		gl_extensions = ptr;
	} else {
		checkGlErrorImpl("Get GL Extensions",__FILE__,__LINE__);
		status = false;
	}

	for(int i=0;i<GL_FEATURES_COUNT;i++){
		gl_features[i] = false;
	}

	if(false){//  major_version >= 4 || gl_extensions.find("GL_ARB_transform_feedback2") != std::string::npos){
		gl_features[GL_TRANSFORM_FEEDBACK_2] = true;
		cout << "GL_ARB_transform_feedback2 available" << endl;
	}

	if(gl_extensions.find("GL_KHR_debug") != std::string::npos){
		glDebugMessageCallback((GLDEBUGPROC)OpenglErrorCallback,nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}//*/
	sFreeTexMemNum = 0;
	if(gl_extensions.find("GL_NVX_gpu_memory_info") != std::string::npos){
		sFreeTexMemNum = 0x9049;
	}
	
	if(gl_extensions.find("GL_ATI_meminfo") != std::string::npos){
		sFreeTexMemNum = 0x87FC;
	}
	return status;
}

int MaxFboColorAttachments(){
	static int numAttachments;
	if(!numAttachments){
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,&numAttachments);
	}
	return numAttachments;
}
#ifndef _WIN32
void DebugBreak(){
//TODO: Do something here!
    cout << "!!DebugBreak Called!!" << endl;
}
#endif
