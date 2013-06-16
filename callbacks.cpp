#include "glutCallbacks.h"
#include "glincludes.h"

int shader=0;


void onReshapeWindow(int w, int h){
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
}



void onRender(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutSwapBuffers();
	//glFlush();
}