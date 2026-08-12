#pragma once

// ImtCore includes
#include "CImtCoreLocalizationInitializer.h"
#include "CImtCoreBaseInitializer.h"
#include "CImtCoreAuthInitializer.h"


inline void InitializeImtCoreAuthorizableServer()
{
	ImtCoreInitLocalizationResources();
	ImtCoreInitBaseResources();
	ImtCoreInitAuthResources();
}
