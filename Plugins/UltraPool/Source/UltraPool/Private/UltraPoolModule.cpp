// ============================================================================
// UltraPool - High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2026 Théo de Nanassy - All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

#include "Modules/ModuleManager.h"


class FUltraPoolModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FUltraPoolModule, UltraPool)
