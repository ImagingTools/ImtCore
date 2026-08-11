#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseUiInitializer.h>
#include <imtcore/CImtCoreAuthUiInitializer.h>
#include <imtcore/CImtCoreLicUiInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientAuthLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBaseUiInit();
	InitializeImtCoreAuthUiInit();
	InitializeImtCoreLicUiInit();
}


} // namespace imtcore