#ifndef GAMENEGINE_H_
#define GAMENEGINE_H_

#include <Theron/Theron.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <unordered_map>
#include <iterator>
#include "Component.h"

typedef boost::uuids::uuid UID;
typedef Component* ComponentPtr;

class GameObject : public Theron::Actor
{
public:

	friend class Component;

	explicit GameObject(Theron::Framework &framework) : Theron::Actor(framework)
	{
		RegisterHandler(this, &GameObject::Handler);
	}

	ComponentPtr GetComponent(int id);
	void AddComponent(const ComponentPtr comp);
	bool HasComponent(int id);

private:
	UID id;
	//Level* level?
	std::unordered_map<int, Component*> components;

	void Handler(const int &message, const Theron::Address from )
	{
		Send(message, from);
	}
};

#endif