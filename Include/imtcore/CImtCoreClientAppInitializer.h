#pragma once

// ImtCore includes
#include "CImtCoreLocalizationInitializer.h"
#include "CImtCoreStyleInitializer.h"
#include "CImtCoreBaseInitializer.h"
#include "CImtCoreAuthInitializer.h"
#include "CImtCoreDeskInitializer.h"
#include "CImtCoreLicInitializer.h"

inline void InitializeImtCoreClientApp()
{
	ImtCoreInitLocalizationResources();
	InitializeImtCoreStyle();
	ImtCoreInitBaseResources();
	ImtCoreInitBaseResources();
	ImtCoreInitAuthResources();
	ImtCoreInitAuthUiResources();
	ImtCoreInitDeskResources();
	ImtCoreInitDeskUiResources();
	ImtCoreInitLicUiResources();
}
