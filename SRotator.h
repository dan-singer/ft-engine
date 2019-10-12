#pragma once
#include "SSystem.h"
class SRotator : public SSystem
{

public:
	virtual void Update(std::vector<Entity*>& entities, float dt) override;

};

