#include <imtcore/CImtCoreClientAppInitializer.h>

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>


namespace imtcore
{


void InitializeImtCoreClientApp()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreStyle();
	InitializeImtCoreBase();
	InitializeImtCoreAuth();
	InitializeImtCoreDesk();
	InitializeImtCoreLic();
}


} // namespace imtcore
