#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseUiInitializer.h>
#include <imtcore/CImtCoreLicUiInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBaseUiInit();
	InitializeImtCoreLicUiInit();
}


} // namespace imtcore