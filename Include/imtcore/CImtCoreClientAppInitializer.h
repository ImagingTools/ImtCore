#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientApp()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	InitializeImtCoreAuth();
	InitializeImtCoreAuthUi();
	InitializeImtCoreDesk();
	InitializeImtCoreDeskUi();
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}


} // namespace imtcore
