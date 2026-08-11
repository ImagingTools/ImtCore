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
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	ImtCoreInitAuthResources();
	ImtCoreInitAuthUiResources();
	InitializeImtCoreDesk();
	InitializeImtCoreDeskUi();
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}
