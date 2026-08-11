#pragma once

// ImtCore includes
#include "CImtCoreLocalizationInitializer.h"
#include "CImtCoreStyleInitializer.h"
#include "CImtCoreBaseInitializer.h"
#include "CImtCoreLicInitializer.h"

inline void InitializeImtCoreClientLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}