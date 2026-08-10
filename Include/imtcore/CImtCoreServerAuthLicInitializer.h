#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreServerAuthLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreAuth();
	InitializeImtCoreLic();
}


} // namespace imtcore
