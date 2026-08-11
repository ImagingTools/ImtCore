#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreClientLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}


} // namespace imtcore