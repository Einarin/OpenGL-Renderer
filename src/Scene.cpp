#include "..\include\Scene.h"
#include "tinyxml2.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;
using namespace tinyxml2;

namespace gl {

	SceneLoader::SceneLoader() :m_didLoad(false) {}

	SceneLoader::SceneLoader(string file, AssetManager& assetManager) : m_didLoad(false) {
		m_didLoad = loadFile(file, assetManager);
	}

	bool SceneLoader::loadFile(string file, AssetManager& assetManager) {
		m_objects.resize(4);
		//m_objects[0].localTransform = rotate(translate(mat4(), vec3(-5.f, -3.f, 0.f)), -20.f, vec3(0.f, 1.f, 0.f));
		m_objects[0].localTransform = scale(translate(mat4(), vec3(-5.f, 4.3f, 4.4f)), vec3(0.5f,0.65f,0.55f));
		m_objects[0].model = assetManager.loadModel("assets/Suit Helmet.fbx");
		m_objects[1].localTransform = rotate(translate(rotate(mat4(), 210.f, vec3(0.f, 1.f, 0.f)), vec3(-5.f, 0.f, -6.f)),-90.f,vec3(1.f,0.f,0.f));
		m_objects[1].model = assetManager.loadModel("assets/falcon3.fbx");
		m_objects[2].localTransform = rotate(rotate(translate(mat4(), vec3(-10.f, 1.f, 6.f)), -20.f, vec3(0.f, 1.f, 0.f)),90.f,vec3(1.f,0.f,0.f));
		m_objects[2].model = assetManager.loadModel("assets/missile3.fbx");
		m_objects[3].localTransform = rotate(translate(mat4(), vec3(-5.f, -10.f, 5.f)),180.f,vec3(0.f,1.f,0.f));
		m_objects[3].model = assetManager.loadModel("assets/MakeHuman/woman.obj");
		m_didLoad = true;
		return true;
	}

}
