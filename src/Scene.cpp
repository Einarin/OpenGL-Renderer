#include "..\include\Scene.h"
#include "tinyxml2.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

using namespace std;
using namespace glm;
using namespace tinyxml2;

//Structure of scene file format
// 

namespace gl {

	Scene::Scene() :m_didLoad(false) {}

	Scene::Scene(string file, AssetManager& assetManager) : m_didLoad(false) {
		m_didLoad = loadFile(file, assetManager);
	}

	void Scene::loadDemo(AssetManager& assetManager) {
		m_models.resize(7);
		//m_objects[0].localTransform = rotate(translate(mat4(), vec3(-5.f, -3.f, 0.f)), -20.f, vec3(0.f, 1.f, 0.f));
		m_models[0].localTransform = scale(translate(mat4(), vec3(-5.f, 4.3f, 4.4f)), vec3(0.5f, 0.65f, 0.55f));
		m_models[0].modelName = "assets/Suit Helmet.fbx";
		m_models[0].model = assetManager.loadModel(m_models[0].modelName);
		m_models[1].localTransform = rotate(translate(rotate(mat4(), 210.f, vec3(0.f, 1.f, 0.f)), vec3(-5.f, 0.f, -6.f)), -90.f, vec3(1.f, 0.f, 0.f));
		m_models[1].modelName = "assets/falcon3.fbx";
		m_models[1].model = assetManager.loadModel(m_models[1].modelName);
		//m_models[2].localTransform = rotate(rotate(translate(mat4(), vec3(-10.f, 1.f, 6.f)), -20.f, vec3(0.f, 1.f, 0.f)), 90.f, vec3(1.f, 0.f, 0.f));
		//m_models[2].modelName = "assets/missile3.fbx";
		//m_models[2].model = assetManager.loadModel(m_models[2].modelName);
		m_models[3].localTransform = rotate(translate(mat4(), vec3(-5.f, -10.f, 5.f)), 180.f, vec3(0.f, 1.f, 0.f));
		m_models[3].modelName = "assets/MakeHuman/woman.obj";
		m_models[3].model = assetManager.loadModel(m_models[3].modelName);
		m_models[4].localTransform = rotate(translate(mat4(), vec3(-15.f, -10.f, 5.f)), 180.f, vec3(0.f, 1.f, 0.f));
		m_models[4].modelName = "assets/MakeHuman/man.obj";
		m_models[4].model = assetManager.loadModel(m_models[4].modelName);
		//m_models[5].localTransform = rotate(translate(rotate(mat4(), 210.f, vec3(0.f, 1.f, 0.f)), vec3(-5.f, 5.f, -6.f)), -90.f, vec3(1.f, 0.f, 0.f));
		m_models[5].localTransform = rotate(translate(mat4(), vec3(0.f, 4.f, 0.f)),90.f,vec3(1.f,0.f,0.f));
		m_models[5].modelName = "assets/turret.fbx";
		m_models[5].model = assetManager.loadModel(m_models[5].modelName);
		/*m_models[6].localTransform = rotate(rotate(translate(mat4(), vec3(10.f, -10.f, 6.f)), 20.f, vec3(0.f, 1.f, 0.f)), -90.f, vec3(1.f, 0.f, 0.f));
		m_models[6].modelName = "assets/fighter.fbx";
		m_models[6].model = assetManager.loadModel(m_models[6].modelName);*/
		m_didLoad = true;
	}

	bool Scene::loadFile(string file, AssetManager& assetManager) {
		XMLDocument doc;
		if (doc.LoadFile(file.c_str()) != XMLError::XML_SUCCESS) {
			return false;
		}
		auto scene = doc.FirstChildElement();
		if (!(scene && scene->Name() == std::string("Scene"))) {
			return false;
		}
		auto sceneModel = scene->FirstChildElement("SceneModel");
		while (sceneModel){
			SceneModel m;
			auto modelName = sceneModel->FirstChildElement("ModelName");
			if (modelName) {
				const char* ptr = modelName->GetText();
				if (ptr) {
					m.modelName = ptr;
				}
				m.model = assetManager.loadModel(m.modelName);
			}
			auto modelMatrix = sceneModel->FirstChildElement("Mat4");
			if (modelMatrix) {
				const char* ptr = modelMatrix->GetText();
				if (ptr) {
					stringstream csv(ptr);
					for (int y = 0; y < 4; y++) {
						for (int x = 0; x < 4; x++) {
							csv >> m.localTransform[x][y];
						}
					}
				}
			}
			m_models.push_back(m);
			sceneModel = sceneModel->NextSiblingElement("SceneModel");
		}
		return true;
	}

	bool Scene::saveFile(string file) {
		FILE* fp = fopen(file.c_str(), "w");
		if (!fp) {
			return false;
		}
		XMLPrinter printer(fp);
		printer.PushHeader(false, true);
		printer.OpenElement("Scene");
		printer.PushAttribute("version", 1);
		for (SceneModel& m : m_models) {
			printer.OpenElement("SceneModel");
			printer.OpenElement("ModelName");
			printer.PushText(m.modelName.c_str());
			printer.CloseElement();
			printer.OpenElement("Mat4");
			for (int y = 0; y < 4; y++) {
				for (int x = 0; x < 4; x++) {
					printer.PushText(m.localTransform[x][y]);
					if (!(x==3 && y==3)) {
						printer.PushText(" ");
					}
				}
			}
			printer.CloseElement();
			printer.CloseElement();
		}
		printer.CloseElement();
		fclose(fp);
		return true;
	}
}
