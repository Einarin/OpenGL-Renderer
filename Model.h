#pragma once
#include "Texture.h"
#include "Light.h"
#include "Mesh.h"
#include <assimp/scene.h> // Output data structure

#include <string>
#include <vector>

namespace gl {
class Model {
protected:
	class ModelPart {
	public:
		ModelPart():parent(nullptr)
		{}
		std::string name;
		std::vector<RenderableMesh> meshes;
		ModelPart* parent;
		glm::mat4 localTranform;
		std::vector<ModelPart> children;
		std::vector<Light*> lights;
		uint32 index; //used for flattening the graph
	};
	class Material {
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 emissive;
		float shininess;
		float shininessStrength;
		float opacity;
		std::string name;
		std::string diffuseFile;
		TexRef diffuseTex;
		std::string specularFile;
		TexRef specularTex;
		std::string normalFile;
		TexRef normalTex;
		std::string displacementFile;
		TexRef displacementTex;
		Material():ambient(0.1f),diffuse(1.f),specular(1.f),
			emissive(0.f),shininess(5.f),shininessStrength(1.f),
			opacity(1.f)
		{}
		bool opaque(){
			return opacity == 1.0f;
		}
		void setupTextures(){
			auto tm = TextureManager::Instance();
			if(!diffuseFile.empty()){
				diffuseTex = tm->texFromFile(diffuseFile);
			}
			if(!specularFile.empty()){
				specularTex = tm->texFromFile(specularFile);
			}
			if(!normalFile.empty()){
				normalTex = tm->texFromFile(normalFile);
			}
			if(!displacementFile.empty()){
				displacementTex = tm->texFromFile(displacementFile);
			}
		}
		void init(){
			if((bool)diffuseTex){
				diffuseTex->init();
			}
			if((bool)specularTex){
				specularTex->init();
			}
			if((bool)normalTex){
				normalTex->init();
			}
			if((bool)displacementTex){
				displacementTex->init();
			}
		}
	};
	std::string filepath;
	std::vector<TexRef> textures;
	std::vector<Light> lights;
	std::vector<Material> materials;
	ModelPart rootPart;
	char* meshbuff;
	bool m_loaded,m_downloaded;
	void loadTextures(const aiScene* scene);
	void buildFromNode(const aiScene* scene, aiNode* node, glm::mat4 transform,ModelPart* currentPart);
	void buildMeshAt(const aiScene* scene, unsigned int meshIndex, Mesh& output); //potential optimization, only build each index once
	void initPart(ModelPart& part);
	void downloadPart(ModelPart& part);
	void drawPart(ModelPart& part,LitTexMvpShader& s);
	bool loadCache(std::string cachefile);
	unsigned int typenum();
public:
	Model(std::string filename);
	~Model();
	static void addProcessing(std::function<void(Model*)> f);
	void save(std::string filename);
	bool ready(){
		return m_loaded && m_downloaded;
	}
	virtual void init();
	virtual void download();
	virtual void draw(LitTexMvpShader& s);
};
}