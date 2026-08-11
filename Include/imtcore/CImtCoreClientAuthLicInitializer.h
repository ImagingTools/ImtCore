#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientAuthLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	InitializeImtCoreAuth();
	InitializeImtCoreAuthUi();
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}


} // namespace imtcore