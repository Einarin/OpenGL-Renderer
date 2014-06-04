#ifndef GAMENEGINE_H_
#define GAMENEGINE_H_

#include <Theron/Theron.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <unordered_map>
#include "Component.h"

typedef boost::uuids::uuid UID;

class GameObject : public Theron::Actor
{
public:
	GameObject(Theron::Framework &framework) : Theron::Actor(framework)
	{
		//RegisterHandle
	}

	Component* GetComponent(UID id);

	void AddComponent(Component *comp);
	bool HasComponent(UID id);
private:
	UID id;
	//Level* level?
	std::unordered_map<UID, Component> components;
};

#endif