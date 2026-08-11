#pragma once

// ImtCore includes
#include <imtcore/CImtCoreBaseInitializer.h>

namespace imtcore
{

/// \todo discuss how to name this function
inline void InitializeImtCoreBaseUiInit()
{
	InitializeImtCoreBase();
	InitializeImtCoreBaseUi();
}


} // namespace imtcore
