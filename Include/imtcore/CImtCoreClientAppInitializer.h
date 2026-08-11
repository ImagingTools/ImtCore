#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseUiInitializer.h>
#include <imtcore/CImtCoreAuthUiInitializer.h>
#include <imtcore/CImtCoreDeskUiInitializer.h>
#include <imtcore/CImtCoreLicUiInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientApp()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBaseUiInit();
	InitializeImtCoreAuthUiInit();
	InitializeImtCoreDeskUiInit();
	InitializeImtCoreLicUiInit();
}


} // namespace imtcore
