#include "AssetManager.h"
#include "ThreadPool.h"
#include "ImageLoader.h"
#include <iostream>

using namespace std;

namespace gl{

	AssetManager* AssetManager::m_self;

std::string AssetManager::resolveLocation(std::string name){
	return name;
}

std::string AssetManager::resolveTextureLocation(std::string name){
	return resolveLocation(name + ".png");
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
					//ptr->save(Model::cachename(filename));
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

/*std::shared_ptr<GlTexture> AssetManager::loadTexture(std::string name, bool sRGB){
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
}*/

std::shared_ptr<Texture2D> AssetManager::loadTextureFromFile(std::string name, bool sRGB){
	auto ptr = m_texture2Cache[name].lock();
	if (ptr.use_count() == 0){
		std::string filename = resolveTextureLocation(name);
		cout << name << " will load from: " << filename << endl;
		//assume png is RGBA for now
		GLuint id;
		glGenTextures(1, &id);
		auto tex = std::shared_ptr<Texture2D>(new Texture2D(id));
		m_texture2Cache[name] = tex;
		IoPool.async([=](){
			glm::ivec2 size;
			GLubyte* data = nullptr;
			int dataLen;
			if (!imageDataFromPngFile(filename, &size, (char**)&data, &dataLen)) {
				std::cout << "failed loading " << filename << "\n";
			}
			glQueue.async([=](){
				tex->bind();
				if (sRGB){
					tex->alloc(GL_RGBA, GL_SRGB8_ALPHA8, size, GL_UNSIGNED_BYTE);
				} else {
					tex->alloc(GL_RGBA, GL_RGBA8, size, GL_UNSIGNED_BYTE);
				}
				checkGlError("Alloc");
				tex->setImage(GL_RGBA, size, data);
				checkGlError("download");
				if (data){
					free(data);
				}
			});
		});
		return tex;
	}
	//already loaded
	return ptr;
}

} //namespace gl
