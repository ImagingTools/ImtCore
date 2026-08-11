#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseUiInitializer.h>
#include <imtcore/CImtCoreAuthUiInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientAuth()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBaseUiInit();
	InitializeImtCoreAuthUiInit();
}


} // namespace imtcore