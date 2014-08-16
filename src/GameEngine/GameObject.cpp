#include "GameObject.h"

template <typename T>
std::shared_ptr<T> GameObject::GetComponent()
{
	std::type_index index(typeid(T));

	if(components.count(index))
	{
		return components[index];
	}

	return NULL;
}

template <typename T>
void  GameObject::AddComponent(const Component* component)
{
	components[std::type_index(typeid(*component))] = component;
}

template <typename T>
bool GameObject::HasComponent()
{
	return components.count(std::type_index(typeid(T)));
}