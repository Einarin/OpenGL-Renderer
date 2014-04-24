#include "glincludes.h"
#include <string>
#include <iostream>

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

void SetupSupport(){
	std::string version((const char*)glGetString(GL_VERSION));
	
	gl_extensions = ((const char*)glGetString(GL_EXTENSIONS));
	int spaceEnd = version.find_last_of(' ');
	int dotEnd = version.find_last_of('.');
	major_version = version[spaceEnd+1] - 0x30; //convert ASCII to number
	minor_version = version[dotEnd+1] - 0x30;

	cout << "OpenGL version " << major_version << '.' << minor_version << " supported" << endl;
	cout << "Vendor is " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer is " << glGetString(GL_RENDERER) << endl;

	for(int i=0;i<GL_FEATURES_COUNT;i++){
		gl_features[i] = false;
	}

	if(major_version >= 4 || gl_extensions.find("GL_ARB_transform_feedback2") != std::string::npos){
		gl_features[GL_TRANSFORM_FEEDBACK_2] = true;
		cout << "GL_ARB_transform_feedback2 available" << endl;
	}

}

int MaxFboColorAttachments(){
	static int numAttachments;
	if(!numAttachments){
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,&numAttachments);
	}
	return numAttachments;
}