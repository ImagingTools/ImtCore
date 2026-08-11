#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientAuth()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	InitializeImtCoreAuth();
	InitializeImtCoreAuthUi();
}


} // namespace imtcore