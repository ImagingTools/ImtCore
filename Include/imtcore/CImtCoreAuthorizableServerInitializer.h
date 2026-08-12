#pragma once


// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>


inline void InitializeImtCoreAuthorizableServer()
{
	ImtCoreInitLocalizationResources();
	ImtCoreInitBaseResources();
	ImtCoreInitAuthSqlResources();
}
