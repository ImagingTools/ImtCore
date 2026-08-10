#include <imtcore/CImtCoreServerLicInitializer.h>

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>


namespace imtcore
{


void InitializeImtCoreServerLic()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreLic();
}


} // namespace imtcore
