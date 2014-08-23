#include "Vertex.h"
#include "VertexAttribBuilder.h"

void VertexTextured::configAttrib(){
	VertexAttribBuilder b;
	b.setSize(sizeof(VertexTextured))
		.attrib(FLOAT_ATTRIB, 3)
		.attrib(FLOAT_ATTRIB, 2)
		.build();
}

void VertexProcedural::configAttrib(){
	VertexAttribBuilder b;
	b.setSize(sizeof(VertexProcedural))
		.attrib(FLOAT_ATTRIB, 3)
		.pad(4)
		.attrib(FLOAT_ATTRIB, 3)
		.pad(4)
		.attrib(FLOAT_ATTRIB, 3)
		.build();
}

void VertexModel::configAttrib(){
	VertexAttribBuilder b;
	b.setSize(sizeof(VertexModel))
		.attrib(FLOAT_ATTRIB, 3)
		.pad(4)
		.attrib(FLOAT_ATTRIB, 3)
		.pad(4)
		.attrib(FLOAT_ATTRIB, 3)
		.pad(4)
		.attrib(FLOAT_ATTRIB, 3)
		.build();
}