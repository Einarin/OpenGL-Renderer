#include "glincludes.h"
#include <string>
#include <iostream>

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
		std::cout << file << ":"<< line <<" GL error "<< error << " at " << step << std::endl;
		//printGlError(("GL error "+ error + " in step " +step + " at " + file + ":%d\r\n").c_str());
		return err;
	}
	else
	{
		//printGlDebug((step+" complete").c_str());
	}
	return 0;
}