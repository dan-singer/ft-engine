#pragma once
#include "Entity.h"

class SSystem
{
public:
	virtual void Update(std::vector<Entity*>& entities, float dt) = 0;
};

