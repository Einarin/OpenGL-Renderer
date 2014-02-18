#include "glincludes.h"
#include "Model.h"
#include "lz4.h"
#include "lz4hc.h"
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/mesh.h>
#include <assimp/ProgressHandler.hpp>
#include <iostream>
#include <fstream>

namespace gl {
using namespace Assimp;

#define VEC_COPY(v1,v2) v1.x=v2.x;v1.y=v2.y;v1.z=v2.z;
#define COLOR_COPY(c1,c2) c1.r=c2.r;c1.g=c2.g;c1.b=c2.b
#define RGBA_COPY(c1,c2) c1.r=c2.r;c1.g=c2.g;c1.b=c2.b;c1.a=c2.a

class CoutProgressHandler : public Assimp::ProgressHandler
{
public:
	CoutProgressHandler()
	{}
	virtual ~CoutProgressHandler()
	{}
	virtual bool Update(float percentage = -1.f)
	{
		if(percentage == -1.f)
			std::cout << "processing..." << std::endl;
		else
			std::cout << percentage << "% complete" << std::endl;
		return true;
	}
};

Model::Model(std::string filename) : filepath(filename){
	std::string cachename = filename.substr(0,filename.find_last_of('.'))+".model";
	std::ifstream cachefile(cachename);
	if(cachefile.is_open()){
		std::cout << "loading cached version of " << filename << std::endl;
		cachefile.close();
		loadCache(cachename);
		return;
	}
	rootPart.name = filename+" root node";
	Assimp::Importer importer;
	std::cout << "loading " << filename << " using assimp..." << std::endl;
	//remove properties we don't care about to aid optimization
	importer.SetPropertyInteger(AI_CONFIG_PP_RRM_EXCLUDE_LIST,
								aiComponent_CAMERAS |
								aiComponent_COLORS );
	importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, 1);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetProgressHandler(new CoutProgressHandler());
	const aiScene* scene = importer.ReadFile(filename
								,aiProcess_CalcTangentSpace 
								| aiProcess_ValidateDataStructure
								| aiProcess_Triangulate 
								| aiProcess_JoinIdenticalVertices
								| aiProcess_GenSmoothNormals 
								| aiProcess_GenUVCoords
								| aiProcess_TransformUVCoords 
								| aiProcess_OptimizeMeshes 
								| aiProcess_ImproveCacheLocality
								| aiProcess_OptimizeGraph 
								| aiProcess_RemoveRedundantMaterials
								| aiProcess_FindDegenerates
								| aiProcess_FindInvalidData
								| aiProcess_SortByPType
								);
	std::cout << "processing " << filename << " scene..." << std::endl;
	if(scene == NULL){
		std::cout << importer.GetErrorString() << std::endl;
		return;
	}
	if(scene->HasTextures()){
		std::cout << filepath << " contains " << scene->mNumTextures << "textures\n";
		loadTextures(scene);
	}
	std::cout << "model has " << scene->mNumLights << " lights" << std::endl;
	if(scene->HasLights()){
		for(int i=0;i<(int)scene->mNumLights;i++){
			aiLight* assLight = scene->mLights[i];
			Light light;
			VEC_COPY(light.position,assLight->mPosition);
			VEC_COPY(light.direction,assLight->mDirection);
			COLOR_COPY(light.ambient,assLight->mColorAmbient);
			COLOR_COPY(light.diffuse,assLight->mColorDiffuse);
			COLOR_COPY(light.specular,assLight->mColorSpecular);
			light.attenuationConstant = assLight->mAttenuationConstant;
			light.attenuationLinear = assLight->mAttenuationLinear;
			light.attenuationQuadratic = assLight->mAttenuationQuadratic;
			light.innerAngle = assLight->mAngleInnerCone;
			light.outerAngle = assLight->mAngleOuterCone;
			switch(assLight->mType){
				case aiLightSource_DIRECTIONAL:
					light.type = Light::Directional;
					break;
				case aiLightSource_POINT:
					light.type = Light::Point;
					break;
				case aiLightSource_SPOT:
					light.type = Light::Spot;
					break;
				default:
					light.type = Light::Undefined;
			}
			light.name = assLight->mName.C_Str();
			lights.push_back(light);
		}
	}
	if(scene->HasMaterials()){
		for(int i=0;i<scene->mNumMaterials;i++){
			aiMaterial* assmat = scene->mMaterials[i];
			aiString name;
			assmat->Get(AI_MATKEY_NAME,name);
			std::cout << "material " << name.C_Str() << std::endl;
			int shadingModel;
			assmat->Get(AI_MATKEY_SHADING_MODEL,shadingModel);
			std::cout << "\tshading model is ";
			switch(shadingModel){
			case aiShadingMode_Flat:
				std::cout << "Flat\n";
				break;
			case aiShadingMode_Gouraud:
				std::cout << "Gouraud\n";
				break;
			case aiShadingMode_Phong:
				std::cout << "Phong\n";
				break;
			default:
				std::cout << "advanced...\n";
			}
			aiString file;
			if(AI_SUCCESS == assmat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0),file))
				std::cout << "Diffuse tex: " << file.C_Str() << std::endl;
			if(AI_SUCCESS == assmat->Get(AI_MATKEY_TEXTURE_NORMALS(0),file))
				std::cout << "Normal tex: " << file.C_Str() << std::endl;
			if(AI_SUCCESS == assmat->Get(AI_MATKEY_TEXTURE_DISPLACEMENT(0),file))
				std::cout << "Diffuse tex: " << file.C_Str() << std::endl;

			/*std::cout << "\tproperty key dump:\n";
			for(int j=0;j<assmat->mNumProperties;j++){
				std::cout <<"\t"<< assmat->mProperties[j]->mKey.C_Str() << " ";
			}*/
		}
	}
	
	buildFromNode(scene, scene->mRootNode, glm::mat4(),&rootPart);
	std::cout << "saving optimized representation to disk...\n";
	save(cachename);
}

void Model::loadTextures(const aiScene* scene){
	for(unsigned int i=0;i<scene->mNumTextures;i++){
		aiTexture* tex = scene->mTextures[i];
		TextureManager* texMan = TextureManager::Instance();
		if(tex->mHeight > 0){ //this is a bitmap texture, stored as unsigned RGBA8888
			textures.push_back(texMan->texFromRGBA8888((char*)(tex->pcData),glm::ivec2(tex->mWidth,tex->mHeight)));
		} else { //texture is in a compressed format, we need to read it ourselves
			if(tex->CheckFormat("png")){
				textures.push_back(texMan->texFromPngBytestream((char*)tex->pcData,tex->mWidth));
			} else {
				std::cout << "Model file " << filepath
					<< " contains unsupported compressed texture type " << tex->achFormatHint << std::endl;
				//we push back an empty texture here anyway so the array indices are correct
				textures.push_back(texMan->unbackedTex(glm::ivec2(0)));
			}
		}
	}
}
//walk the node heirarchy
void Model::buildFromNode(const aiScene* scene, aiNode* node, glm::mat4 transform, ModelPart* currentPart){
	std::cout << "processing node " << node->mName.C_Str() << "..." << std::endl;
	std::vector<Light*> currentLights;
	glm::mat4 nodeTransform = *((glm::mat4*)&node->mTransformation);
	for(auto light = lights.begin();light != lights.end();light++){
		if(light->name == node->mName.C_Str()){
			currentLights.push_back(&*light);
		}
	}
	for(auto it = currentLights.begin();it!=currentLights.end();it++){
		currentPart->lights.push_back(*it);
	}
	unsigned int meshCount=0;
	unsigned int offset = currentPart->meshes.size();
	currentPart->meshes.resize(offset+node->mNumMeshes);
	while(meshCount < node->mNumMeshes){
		buildMeshAt(scene, node->mMeshes[meshCount],currentPart->meshes[offset+meshCount]);
		meshCount++;
	}
	for(unsigned int i=0;i<node->mNumChildren;i++){
		currentPart->children.push_back(ModelPart());
		currentPart->children.back().parent = currentPart;
		buildFromNode(scene,node->mChildren[i],nodeTransform * transform,&currentPart->children.back());
	}
}

void Model::buildMeshAt(const aiScene* scene, unsigned int meshIndex, Mesh& output){
	aiMesh& aim = *scene->mMeshes[meshIndex];
	if (aim.mPrimitiveTypes & ( aiPrimitiveType_LINE | aiPrimitiveType_POINT | aiPrimitiveType_POLYGON )){
		std::cout << "mesh " << aim.mName.C_Str() <<" has unsupported primitive types, expect rendering errors" << std::endl;
	}
	output.name = aim.mName.C_Str();
	output.hasNormals = aim.HasNormals();
	output.hasTangents = aim.HasTangentsAndBitangents();
	output.numUVChannels = aim.GetNumUVChannels();
	output.numVertexColorChannels = aim.GetNumColorChannels();
	output.materialIndex = aim.mMaterialIndex;
	output.drawCount = aim.mNumFaces;
	output.indices.clear();
	output.indices.reserve(aim.mNumFaces*3);
	for(int i=0;i<aim.mNumFaces;i++){
		for(int j=0;j<aim.mFaces[j].mNumIndices;j++){
			output.indices.push_back(aim.mFaces[i].mIndices[j]);
		}
	}
	for(int i=0;i<AI_MAX_NUMBER_OF_TEXTURECOORDS;i++){
		output.numUVComponents[i] = aim.mNumUVComponents[i];
	}
	output.vertices.resize(aim.mNumVertices);
	for(unsigned int i=0;i<aim.mNumVertices;i++){
		vertex& v = output.vertices[i];
		VEC_COPY(v.pos,aim.mVertices[i]);
		v.pos.w = 1.f;
		if(output.hasNormals){
			VEC_COPY(v.norm,aim.mNormals[i]);
		}
		if(output.hasTangents){
			VEC_COPY(v.tan,aim.mTangents[i]);
			VEC_COPY(v.bitan,aim.mBitangents[i]);
		}
		for(int j=0;j<AI_MAX_NUMBER_OF_TEXTURECOORDS;j++){
			if(aim.HasTextureCoords(j)){
				VEC_COPY(v.tc[j],aim.mTextureCoords[j][i]);
			}
		}
		for(int j=0;j<AI_MAX_NUMBER_OF_COLOR_SETS;j++){
			if(aim.HasVertexColors(j)){
				RGBA_COPY(v.colors[j],aim.mColors[j][i]);
			}
		}
	}
}

void Model::initPart(ModelPart& part)
{
	for(auto m = part.meshes.begin();m != part.meshes.end();m++){
		m->init();
	}
	for(auto c = part.children.begin();c != part.children.end();c++){
		initPart(*c);
	}
}
void Model::downloadPart(ModelPart& part)
{
	for(auto m = part.meshes.begin();m != part.meshes.end();m++){
		m->download();
	}
	for(auto c = part.children.begin();c != part.children.end();c++){
		downloadPart(*c);
	}
}
void Model::drawPart(ModelPart& part)
{
	for(auto m = part.meshes.begin();m != part.meshes.end();m++){
		m->draw();
	}
	for(auto c = part.children.begin();c != part.children.end();c++){
		drawPart(*c);
	}
}

void Model::init()
{
	initPart(rootPart);
}
void Model::download(){
	downloadPart(rootPart);
}
void Model::draw(){
	drawPart(rootPart);
}
struct FlatModel{
	uint32 nameoff;
	uint32 parentind;
	glm::mat4 localTransform;
	uint32 meshlistoff;
	uint32 meshlistsize;
	uint32 childlistoff;
	uint32 childlistsize;
	uint32 lightlistoff;
	uint32 lightlistsize;
};
struct Header{
	uint32 type;
	uint32 filesize;
	uint32 flatlistoff;
	uint32 flatlistsize;
	uint32 namebuffoff;
	uint32 childbuffoff;
	uint32 meshindoff;
	uint32 meshbuffoff;
	uint32 compressedmeshsize;
};
void Model::save(std::string filename){
	//first walk the tree to assign indexes to nodes and determine buffer sizes
	std::vector<ModelPart*> stack; //used as temp stack
	stack.push_back(&rootPart);
	uint32 nodecount = 0;
	uint32 namebuffsize = 0;
	uint32 childbuffsize = 0;
	uint32 lightindbuffsize = 0;
	while(stack.size() > 0){
		ModelPart* current = stack.back();
		stack.pop_back();
		current->index = nodecount++;
		namebuffsize += current->name.size()+1; //+1 for trailing \0
		childbuffsize += current->children.size();
		lightindbuffsize += current->lights.size();
		for(int i=0; i<current->children.size();i++){
			stack.push_back(&current->children[i]);
		}
	}
	//now we have enough information to allocate arrays
	FlatModel* flatlist = new FlatModel[nodecount];
	char* namebuff = new char[namebuffsize];
	uint32* childbuff = new uint32[childbuffsize];
	//uint32* lightindbuff = new uint32[lightindbuffsize];
	uint32 namebuffpos = 0;
	uint32 childbuffpos = 0;
	//uint32 lightindbuffpos = 0;

	//we build the mesh collection lazily
	uint32 meshpos = 0;
	uint32 meshind = 0;
	std::vector<char*> meshbuffs;
	std::vector<uint32> meshbuffsizes;
	std::vector<uint32> meshoffsets;
	meshbuffs.reserve(nodecount); //cut down on reallocs

	//now walk again to build our flattened list
	stack.push_back(&rootPart);
	while(stack.size() > 0){
		ModelPart* current = stack.back();
		stack.pop_back();
		FlatModel* m = &flatlist[current->index];
		if(current->parent != NULL){ //root has null par
			m->parentind = current->parent->index;
		} else {
			m->parentind = 0; //just set it to 0 so it's initialized
		}
		m->nameoff = namebuffpos;
		m->localTransform = current->localTranform;
		strcpy(namebuff+namebuffpos,current->name.c_str());
		namebuffpos += current->name.size()+1;

		m->meshlistoff = meshind;
		for(int i=0;i<current->meshes.size();i++){
			meshbuffs.push_back(nullptr);
			uint32 size = current->meshes[i].serialize(&meshbuffs.back());
			meshoffsets.push_back(meshpos);
			meshbuffsizes.push_back(size);
			meshpos += size;
			meshind++;
		}
		m->meshlistsize = current->meshes.size();

		m->childlistoff = childbuffpos;
		for(int i=0; i<current->children.size();i++){
			stack.push_back(&current->children[i]);
			childbuff[childbuffpos+i] = current->children[i].index;
		}
		childbuffpos += current->children.size();
		m->childlistsize = current->children.size();
	}

	char* combinedmeshes = new char[meshpos];
	char* compressedmeshes = new char[LZ4_compressBound(meshpos)];
	uint32 off = 0;
	for(int i=0;i<meshind;i++){
		//file.write(meshbuffs[i],meshbuffsizes[i]);
		memcpy(combinedmeshes+off,meshbuffs[i],meshbuffsizes[i]);
		off += meshbuffsizes[i];
		delete[] meshbuffs[i];
	}
	int compressedsize = LZ4_compressHC(combinedmeshes,compressedmeshes,meshpos);

	//now we have all our buffers, fill out header and write to disk
	Header h;
	//only used compressed data if it succeeded and is smaller than uncompressed
	h.type = ((compressedsize > 0) && (compressedsize < meshpos)) ? 1 : 0;
	if(h.type){
		h.compressedmeshsize = compressedsize;
	} else {
		h.compressedmeshsize = 0;
	}
	h.flatlistoff = sizeof(Header);
	h.flatlistsize = nodecount;
	h.namebuffoff = h.flatlistoff + nodecount * sizeof(FlatModel);
	h.childbuffoff = h.namebuffoff + namebuffsize * sizeof(char);
	h.meshindoff = h.childbuffoff + childbuffsize * sizeof(uint32);
	h.meshbuffoff = h.meshindoff + meshoffsets.size() * sizeof(uint32 );
	h.filesize = h.meshbuffoff + meshpos;
	std::ofstream file(filename,std::ofstream::binary);
	file.write(reinterpret_cast<const char*>(&h),sizeof(Header));
	file.write(reinterpret_cast<const char*>(flatlist),nodecount* sizeof(FlatModel));
	delete[] flatlist;
	file.write(namebuff,namebuffsize);
	delete[] namebuff;
	file.write(reinterpret_cast<const char*>(childbuff),childbuffsize * sizeof(uint32));
	delete[] childbuff;
	file.write(reinterpret_cast<const char*>(&meshoffsets[0]),meshoffsets.size() * sizeof(uint32));
	if(h.type){
		file.write(compressedmeshes,compressedsize);
	} else {
		file.write(combinedmeshes,meshpos);
	}
	file.flush();
	file.close();
}

void Model::loadCache(std::string filename){
	std::ifstream file(filename, std::ifstream::binary);
	Header h;
	file.read(reinterpret_cast<char*>(&h),sizeof(Header));
	char* flatbuff = new char[h.namebuffoff - h.flatlistoff];
	file.read(flatbuff,h.namebuffoff - h.flatlistoff);
	char* namebuff = new char[h.childbuffoff - h.namebuffoff];
	file.read(namebuff,h.childbuffoff - h.namebuffoff);
	uint32* childbuff = reinterpret_cast<uint32*>(new char[h.meshindoff - h.childbuffoff]);
	file.read(reinterpret_cast<char*>(childbuff),h.meshindoff - h.childbuffoff);
	uint32* meshindbuff = reinterpret_cast<uint32*>(new char[h.meshbuffoff - h.meshindoff]);
	file.read(reinterpret_cast<char*>(meshindbuff),h.meshbuffoff - h.meshindoff);
	char* meshbuff = new char[h.filesize - h.meshbuffoff];
	if(h.type){
		char* compressedbuff = new char[h.compressedmeshsize];
		file.read(compressedbuff,h.compressedmeshsize);
		LZ4_decompress_fast(compressedbuff,meshbuff,h.filesize - h.meshbuffoff);
	} else {
		file.read(meshbuff,h.filesize - h.meshbuffoff);
	}
	file.close();
	FlatModel* flatlist = reinterpret_cast<FlatModel*>(flatbuff);
	//build graph from flatlist
	std::vector<ModelPart*> workstack;
	workstack.push_back(&rootPart);
	rootPart.index = 0;
	while(workstack.size() > 0){
		ModelPart* current = workstack.back();
		workstack.pop_back();
		int i = current->index;
		current->name = namebuff+flatlist[i].nameoff;
		for(int j = flatlist[i].meshlistoff;j<flatlist[i].meshlistoff+flatlist[i].meshlistsize;j++){
			current->meshes.emplace_back(RenderableMesh());
			current->meshes.back().deserialize(meshbuff+meshindbuff[j]);
		}
		current->localTranform = flatlist[i].localTransform;
		for(int j= flatlist[i].childlistoff;j<flatlist[i].childlistoff+flatlist[i].childlistsize;j++){
			current->children.emplace_back(ModelPart());
			current->children.back().index = childbuff[j];
			current->children.back().parent = current;
			workstack.push_back(&current->children.back());
		}
	}
}

} //namespace gl