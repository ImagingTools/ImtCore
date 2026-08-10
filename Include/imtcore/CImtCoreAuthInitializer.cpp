#include <imtcore/CImtCoreAuthInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreAuthResourcesInitializer.h>
#include <imtcore/CImtCoreAuthThemeInitializer.h>
#include <imtcore/CImtCoreAuthQmlInitializer.h>
#include <imtcore/CImtCoreAuthSdlInitializer.h>


namespace imtcore
{


void CImtCoreAuthInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreAuthResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthThemeInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthSdlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
