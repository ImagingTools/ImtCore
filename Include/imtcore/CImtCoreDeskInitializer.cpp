#include <imtcore/CImtCoreDeskInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreDeskResourcesInitializer.h>
#include <imtcore/CImtCoreGuiThemeInitializer.h>
#include <imtcore/CImtCoreDeskQmlInitializer.h>
#include <imtcore/CImtCoreDomainSdlInitializer.h>


namespace imtcore
{


void CImtCoreDeskInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreDeskResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreGuiThemeInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDeskQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDomainSdlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
