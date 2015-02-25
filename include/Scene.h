#pragma once
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "Model.h"
#include "AssetManager.h"

namespace gl {

	class Scene {
	public:
		struct SceneModel {
			glm::mat4 localTransform;
			std::shared_ptr<Model> model;
		};
	protected:
		std::vector<SceneModel> m_objects;
	public:
		Scene() = default;
		virtual ~Scene() = default;
		std::vector<SceneModel>::iterator begin() {
			return m_objects.begin();
		}
		std::vector<SceneModel>::iterator end() {
			return m_objects.end();
		}
	};

	class SceneLoader : public Scene {
	protected:
		struct SceneEntry {
			glm::mat4 localTransform;
			std::string modelFile;
		};
		bool m_didLoad;
	public:
		SceneLoader();
		SceneLoader(std::string file, AssetManager& assetManager);
		virtual ~SceneLoader() = default;
		bool loadFile(std::string file, AssetManager& assetManager);
		bool hasLoaded() {
			return m_didLoad;
		}
	};

} //namespace gl
