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
			std::string modelName;
			std::shared_ptr<Model> model;
		};
	protected:
		std::vector<SceneModel> m_models;
		bool m_didLoad;
	public:
		std::vector<SceneModel>::iterator begin() {
			return m_models.begin();
		}
		std::vector<SceneModel>::iterator end() {
			return m_models.end();
		}
		Scene();
		Scene(std::string file, AssetManager& assetManager);
		virtual ~Scene() = default;
		void loadDemo(AssetManager& assetManager);
		bool loadFile(std::string file, AssetManager& assetManager);
		bool hasLoaded() {
			return m_didLoad;
		}
		bool saveFile(std::string filename);
	};

} //namespace gl
