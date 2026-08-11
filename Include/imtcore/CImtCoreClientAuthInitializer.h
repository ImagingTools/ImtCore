#pragma once

// ImtCore includes
#include "CImtCoreLocalizationInitializer.h"
#include "CImtCoreStyleInitializer.h"
#include "CImtCoreBaseInitializer.h"
#include "CImtCoreAuthInitializer.h"

inline void InitializeImtCoreClientAuth()
{
	ImtCoreInitLocalizationResources();
	InitializeImtCoreStyle();
	ImtCoreInitBaseResources();
	ImtCoreInitBaseResources();
	ImtCoreInitAuthResources();
	ImtCoreInitAuthUiResources();
}