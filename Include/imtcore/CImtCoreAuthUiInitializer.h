#pragma once

// ImtCore includes
#include <imtcore/CImtCoreAuthInitializer.h>

namespace imtcore
{

/// \todo discuss how to name this function
inline void InitializeImtCoreAuthUiInit()
{
	InitializeImtCoreAuth();
	InitializeImtCoreAuthUi();
}


} // namespace imtcore
