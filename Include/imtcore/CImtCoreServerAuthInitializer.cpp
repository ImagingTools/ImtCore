#include <imtcore/CImtCoreServerAuthInitializer.h>

// ImtCore includes
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>


namespace imtcore
{


void InitializeImtCoreServerAuth()
{
	InitializeImtCoreLocalization();
	InitializeImtCoreBase();
	InitializeImtCoreAuth();
}


} // namespace imtcore
