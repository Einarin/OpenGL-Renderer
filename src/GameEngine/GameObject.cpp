#include "GameObject.h"

ComponentPtr GameObject::GetComponent(int id)
{
	return components[id];
}

void  GameObject::AddComponent(const ComponentPtr comp)
{
	return;
}

bool GameObject::HasComponent(int id)
{
	return components.count(id) != 0;
}