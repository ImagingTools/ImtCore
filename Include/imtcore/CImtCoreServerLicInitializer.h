#pragma once

// ImtCore includes
#include "CImtCoreLocalizationInitializer.h"
#include "CImtCoreBaseInitializer.h"
#include "CImtCoreLicInitializer.h"

inline void InitializeImtCoreServerLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreLic();
}
