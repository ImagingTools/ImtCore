#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreServerAuthTenant()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreAuth();
	InitializeImtCoreAuthTenant();
}


} // namespace imtcore