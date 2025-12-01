#pragma once

#include "Modules/ModuleManager.h"

class FPuzzleModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};