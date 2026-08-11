#pragma once

// ImtCore includes
#include <imtcore/CImtCoreDeskInitializer.h>

namespace imtcore
{

/// \todo discuss how to name this function
inline void InitializeImtCoreDeskUiInit()
{
	InitializeImtCoreDesk();
	InitializeImtCoreDeskUi();
}


} // namespace imtcore
