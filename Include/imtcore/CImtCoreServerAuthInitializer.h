#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreServerAuth()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreAuth();
}


} // namespace imtcore
