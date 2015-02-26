#include "AssetManager.h"
#include "ThreadPool.h"
#include <iostream>

using namespace std;

namespace gl{

std::string AssetManager::resolveLocation(std::string name){
	return name;
}

shared_ptr<Model> AssetManager::loadModel(string name){
	auto ptr = m_modelCache[name].lock();
	if(ptr.use_count() == 0){ //no object or old object expired
		m_loadsInFlight++;
		std::string filename = resolveLocation(name);
		cout << name << " was loaded from " << filename << endl;
		ptr = shared_ptr<Model>(new Model());
		ptr->setFilePath(filename);
		m_modelCache[name] = ptr;
		//asynchronous loading FTW!
		CpuPool.async([=](){
			if (ptr->open(filename)){
				//successfully got file from disk
				//presumably we want to use the object we got so queue loading it to the GPU
				auto p = ptr; //There must be a better fix for this
				glQueue.async([=](){
					p->init();
					p->download();
					m_loadsInFlight--;
				});
				if(!ptr->wasCached()){
					std::cout << "saving optimized representation to disk...\n";
					ptr->save(Model::cachename(filename));
					std::cout << Model::cachename(filename) << " saved\n";
				}
			} else {
				cout << "request for asset " << filename << " failed" << endl;
				m_loadsInFlight--;
			}
		});
	} else {
		cout << name << " already resident" << endl;
	}
	return ptr;
}

std::shared_ptr<GlTexture> AssetManager::loadTexture(std::string name, bool sRGB){
	auto ptr = m_textureCache[name].lock();
	if (ptr.use_count() == 0){
		std::string filename = resolveLocation(name);
		cout << name << " was loaded from " << filename << endl;
		auto tex = std::shared_ptr<FileBackedGlTexture2D>(new FileBackedGlTexture2D());
		tex->filename = filename;
		tex->sRGB = sRGB;
		m_textureCache[name] = tex;
		return tex;
	}
	//already loaded
	return ptr;
}

} //namespace gl
