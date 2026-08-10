#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{


inline void InitializeImtCoreServerLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreLic();
}


} // namespace imtcore
