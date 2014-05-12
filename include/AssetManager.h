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
public:
	std::shared_ptr<Model> loadModel(std::string filename);
};

}
