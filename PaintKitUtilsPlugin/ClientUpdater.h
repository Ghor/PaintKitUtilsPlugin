#pragma once

#include "platform.h"
#include "game/shared/igamesystem.h"

class CompositorQueueSystem : public CAutoGameSystemPerFrame
{
	CompositorQueueSystem(const char* name) :
		CAutoGameSystemPerFrame(name)
	{
	}

	virtual void Update(float frameTime) override
	{
		Update
	}
};