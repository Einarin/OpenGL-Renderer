#pragma once
#include "Texture.h"
#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Animation.h"
#include <assimp/scene.h> // Output data structure
#include <string>
#include <vector>
#include <map>

namespace gl {
	
class Model {
protected:
	struct Bone {
		glm::mat4 model2LocalXform;
		glm::mat4 mesh2BindXform;
		glm::mat4 finalPosition(glm::mat4 animationXform) {
			return model2LocalXform * animationXform * mesh2BindXform;
		}
	};
	class ModelPart {
	public:
		ModelPart():parent(nullptr)
		{}
		std::string name;
		std::vector<RenderableMesh> meshes;
		ModelPart* parent;
		glm::mat4 localTransform;
		std::vector<ModelPart> children;
		std::vector<int> lights;
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
				//normals are not sRGB encoded
				normalTex = tm->texFromFile(normalFile,false);
			}
			if(!displacementFile.empty()){
				//displacements are not sRGB encoded
				displacementTex = tm->texFromFile(displacementFile,false);
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
	std::vector<Bone> bones;
	std::vector<Animation> animations;
	ModelPart rootPart;
	std::map<std::string, int> boneMap;
	char* meshbuff;
	bool m_loaded,m_downloaded,m_cached,m_hasBones;
	void loadTextures(const aiScene* scene);
	void buildFromNode(const aiScene* scene, aiNode* node, glm::mat4 transform,ModelPart* currentPart);
	void buildMeshAt(const aiScene* scene, unsigned int meshIndex, Mesh& output); //potential optimization, only build each index once
	void initPart(ModelPart& part);
	void downloadPart(ModelPart& part);
	void drawPart(ModelPart& part,LitTexMvpShader& s,const glm::mat4& parentTransform);
	bool loadCache(std::string cachefile);
	void calcAABB();
	unsigned int typenum();
public:
	inline void setFilePath(std::string filename) {
		filepath = filename;
	}
	glm::mat4 ModelMatrix;
	AABB3 BoundingBox;
	Model();
	Model(std::string filename);
	~Model();
	static void addProcessing(std::function<void(Model*)> f);
	static inline std::string cachename(std::string filename){
		return filename+".mcache";
	}
	bool open(std::string filename);
	void save(std::string filename);
	inline bool ready(){
		return m_loaded && m_downloaded;
	}
	inline bool wasCached(){
		return m_cached;
	}
	void init();
	void download();
	void draw(LitTexMvpShader& s);
	void drawBoundingBoxes(Camera* c);
	int getAnimationCount() {
		return animations.size();
	}
	std::string getAnimationName(int index) {
		return animations[index].name;
	}
	int getSkinningBufferCount() {
		return bones.size();
	}
	void setSkinningBuffer(int animationIndex, double time, glm::mat4* buffer);
};

} //namespace gl