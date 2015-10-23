#include "Mesh.h"
#include "VertexAttribBuilder.h"

enum flags{
	HAS_NORMAL = 0x1,
	HAS_TANGENT = 0x2
};

using namespace glm;

namespace gl{
uint32 Mesh::serialize(char** inbuff){
	uint32 bufflen = sizeof(MeshHeader)
			  + (name.size()+1)*sizeof(char)
			  + vertices.sizeInBytes()
			  + indSize * sizeof(unsigned int);
	char* buff = new char[bufflen];
	*inbuff = buff;
	MeshHeader* head = new(buff) MeshHeader;
	head->flags = hasNormals ? HAS_NORMAL : 0;
	head->flags |= hasTangents ? HAS_TANGENT : 0;
	head->drawCount = drawCount;
	head->numVertexColorChannels = numVertexColorChannels;
	head->numUVChannels = numUVChannels;
	head->materialIndex = materialIndex;
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		head->numUVComponents[i] = numUVComponents[i];
	}
	head->nameoff = sizeof(MeshHeader);
	strcpy((buff+head->nameoff),name.c_str());
	head->vertoff = sizeof(MeshHeader) + (name.size()+1)*sizeof(char);
	//TODO:This should probably be replaced by explicit serialization support in VertexBuffer so it's more robust
	head->vertCount = vertices.vertexCount();
	head->vertsize = vertices.vertSizeBytes();
	memcpy(buff + head->vertoff, vertices.buffPtr(), vertices.sizeInBytes());
	head->indoff = sizeof(MeshHeader)
			  + (name.size()+1)*sizeof(char)
			  + vertices.sizeInBytes();
	head->indsize = indSize;
	memcpy(buff+head->indoff,&indices[0],indSize * sizeof(unsigned int));
	return bufflen;
}

bool Mesh::deserialize(char* buff){
	//make sure we don't already have data
	assert(vertSize == 0);
	assert(indSize == 0);
	MeshHeader* head = reinterpret_cast<MeshHeader*>(buff);
	hasNormals = (head->flags & HAS_NORMAL) != 0;
	hasTangents = (head->flags & HAS_TANGENT) != 0;
	drawCount = head->drawCount;
	numVertexColorChannels = head->numVertexColorChannels;
	numUVChannels = head->numUVChannels;
	materialIndex = head->materialIndex;
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		numUVComponents[i] = head->numUVComponents[i];
	}
	name = buff+head->nameoff;
	ownsBuffers = false;
	//TODO:This should probably be replaced by explicit serialization support in VertexBuffer so it's more robust
	VertexBufferBuilder vbb;
	vbb.hasNormal(hasNormals)
		.hasTangent(hasTangents)
		.hasTexCoord3D(numUVChannels) //shares Model's assumption that all UV channels are 3D
		.hasVertColor(numVertexColorChannels)
		.vertexCount(head->vertCount);
	vertices = vbb.fillUnownedBuffer(buff+head->vertoff);
	vertSize = vertices.vertSizeBytes();
	if (head->vertsize != vertSize) { //If this fails than our computed VertexBuffer didn't match the original storing one. This shouldn't ever happen...
		std::cout << "Computed vertex size " << head->vertsize << " doesn't match stored size " << vertSize << "!\nThis is a bug in Mesh Serialization. " << __FILE__ << ":" << __LINE__ << std::endl;
		vertices = VertexBuffer(); //clear vertex buffer
		vertSize = 0;
		return false;
	}
	indices = (unsigned int*)(buff+head->indoff);
	indSize = head->indsize;
	return true;
}

void Mesh::copy(const Mesh& other)
{
	//who calls this anyway? making a deep copy of a mesh seems like a bad idea...
	ownsBuffers = true;
	vertSize = other.vertSize;
	vertices = other.vertices.duplicate();
	indSize = other.indSize;
	indices = new unsigned int[indSize];
	memcpy(&indices[0],&other.indices[0],indSize * sizeof(unsigned int));
	name = other.name;
	hasNormals = other.hasNormals;
	hasTangents = other.hasTangents;
	drawCount = other.drawCount;
	numVertexColorChannels = other.numVertexColorChannels;
	numUVChannels = other.numUVChannels;
	materialIndex = other.materialIndex;
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		numUVComponents[i] = other.numUVComponents[i];
	}
}

void Mesh::calcAABB(){
	if(vertSize > 0){
		vec3 minp,maxp;
		minp = maxp = vertices[0].pos();
		for(int i=1;i<vertSize;i++){
			minp = min(minp,vertices[i].pos());
			maxp = max(maxp,vertices[i].pos());
		}
		BoundingBox = AABB3(minp,maxp);
	}
}

void RenderableMesh::init(){
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	checkGlError("gen");
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkGlError("bind");
	/*VertexAttribBuilder b;
	b.setSize(sizeof(vertex))
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3)
		.pad(4)
		.attrib(FLOAT_ATTRIB,3).build();
	/*glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);*/
	vertices.configureAttributes();
	checkGlError("attribs");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	checkGlError("init");
}

void RenderableMesh::download(){
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkGlError("glBindBuffer verts");
	glBufferData(GL_ARRAY_BUFFER, vertices.sizeInBytes(), vertices.buffPtr(), GL_STATIC_DRAW);
#ifdef _DEBUG
	glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	checkGlError("glBindBuffer elements");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indSize * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	checkGlError("glBufferData elements");
#ifdef _DEBUG
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
#endif
	checkGlError("download complete");
}

void RenderableMesh::draw(){
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,drawCount*3,GL_UNSIGNED_INT,(GLvoid*)0);
	checkGlError("drawelements");
	glBindVertexArray(0);
}

}//namespace gl