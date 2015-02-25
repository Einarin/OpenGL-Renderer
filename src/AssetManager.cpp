#include "AssetManager.h"
#include "ThreadPool.h"
#include <iostream>

using namespace std;

namespace gl{

shared_ptr<Model> AssetManager::loadModel(string filename){
	auto ptr = mcache[filename].lock();
	if(ptr.use_count() == 0){ //no object or old object expired
		m_loadsInFlight++;
		cout << filename << " will load from disk" << endl;
		ptr = shared_ptr<Model>(new Model());
		ptr->setFilePath(filename);
		mcache[filename] = ptr;
		//asynchronous loading FTW!
		CpuPool.async([=](){
			if(ptr->open(filename)){
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
		cout << filename << " already resident" << endl;
	}
	return ptr;
}

} //namespace gl
