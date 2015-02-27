#pragma once
#include <memory>
#include <string>
#include <map>
#include "Model.h"
#include "Texture.h"

namespace gl{

class AssetManager
{
protected:
	std::map<std::string, std::weak_ptr<Model>> m_modelCache;
	std::map<std::string, std::weak_ptr<GlTexture>> m_textureCache;
	int m_loadsInFlight;
public:
	inline AssetManager() :m_loadsInFlight(0)
	{}
	inline int numberOfLoadsInFlight() {
		return m_loadsInFlight;
	}
	std::string resolveLocation(std::string name);
	std::shared_ptr<Model> loadModel(std::string name);
	std::shared_ptr<GlTexture> loadTexture(std::string name, bool sRGB=true);
};

}
