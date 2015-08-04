#pragma once
#include <memory>
#include <string>
#include <map>
#include "Model.h"
// "Texture.h"
#include "Texture2.h"

namespace gl{

class AssetManager
{
protected:
	std::map<std::string, std::weak_ptr<Model>> m_modelCache;
	std::map<std::string, std::weak_ptr<GlTexture>> m_textureCache;
	std::map<std::string, std::weak_ptr<Texture2D>> m_texture2Cache;
	int m_loadsInFlight;
	std::string resolveTextureLocation(std::string name);
	static AssetManager* m_self;
public:
	inline AssetManager() :m_loadsInFlight(0)
	{
		m_self = this;
	}
	inline int numberOfLoadsInFlight() {
		return m_loadsInFlight;
	}
	static AssetManager* Instance() { return m_self; }
	std::string resolveLocation(std::string name);
	std::shared_ptr<Model> loadModel(std::string name);
	//std::shared_ptr<GlTexture> loadTexture(std::string name, bool sRGB=true);
	std::shared_ptr<Texture2D> loadTextureFromFile(std::string name, bool sRGB = true);
};

}
