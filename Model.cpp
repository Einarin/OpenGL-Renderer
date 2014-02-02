#include "glincludes.h"
#include "Model.h"
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/ProgressHandler.hpp>
#include <iostream>

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
	rootPart.name = filename+" root node";
	Assimp::Importer importer;
	std::cout << "loading " << filename << " using assimp..." << std::endl;
	//remove properties we don't care about to aid optimization
	importer.SetPropertyInteger(AI_CONFIG_PP_RRM_EXCLUDE_LIST,
								aiComponent_CAMERAS |
								aiComponent_COLORS );
	importer.SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE, 1);

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

} //namespace gl