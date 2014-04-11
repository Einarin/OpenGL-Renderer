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
	std::string filepath;
	std::vector<TexRef> textures;
	std::vector<Light> lights;
	ModelPart rootPart;
	bool m_loaded,m_downloaded;
	void loadTextures(const aiScene* scene);
	void buildFromNode(const aiScene* scene, aiNode* node, glm::mat4 transform,ModelPart* currentPart);
	void buildMeshAt(const aiScene* scene, unsigned int meshIndex, Mesh& output); //potential optimization, only build each index once
	void initPart(ModelPart& part);
	void downloadPart(ModelPart& part);
	void drawPart(ModelPart& part);
	void loadCache(std::string cachefile);
public:
	Model(std::string filename);
	void save(std::string filename);
	bool ready(){
		return m_loaded && m_downloaded;
	}
	virtual void init();
	virtual void download();
	virtual void draw();
};
}