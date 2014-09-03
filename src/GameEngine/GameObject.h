#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <Theron/Theron.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/variant.hpp>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <iterator>
#include "Component.h"

typedef boost::uuids::uuid UID;

class GameObject : public Theron::Actor
{
public:
	friend class Component;

	explicit GameObject(Theron::Framework &framework) : Theron::Actor(framework)
	{
		RegisterHandler(this, &GameObject::Handler);
	}

	template <typename T>
	std::shared_ptr<T> GetComponent();

	template <typename T>
	void AddComponent(const Component* component);

	template <typename T>
	bool HasComponent();

	virtual GameObject* factory() = 0;

private:
	UID id;

	//boost::variant<

	std::unordered_map<std::type_index, std::shared_ptr<Component> > components;

	void Handler(const int &message, const Theron::Address from )
	{
		Send(message, from);
	}
};

#endif