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
	std::map<std::string,std::weak_ptr<Model>> mcache;
	int m_loadsInFlight;
public:
	inline AssetManager() :m_loadsInFlight(0)
	{}
	inline int numberOfLoadsInFlight() {
		return m_loadsInFlight;
	}
	std::shared_ptr<Model> loadModel(std::string filename);
};

}
