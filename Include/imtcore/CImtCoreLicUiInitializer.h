#pragma once

// ImtCore includes
#include <imtcore/CImtCoreLicInitializer.h>

namespace imtcore
{

/// \todo discuss how to name this function
inline void InitializeImtCoreLicUiInit()
{
	InitializeImtCoreLic();
	InitializeImtCoreLicUi();
}


} // namespace imtcore
