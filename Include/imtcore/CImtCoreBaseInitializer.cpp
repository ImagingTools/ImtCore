#include <imtcore/CImtCoreBaseInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreBaseResourcesInitializer.h>
#include <imtcore/CImtCoreBaseQmlInitializer.h>
#include <imtcore/CImtCoreBaseSdlInitializer.h>
#include <imtcore/CImtCoreStyleResourcesInitializer.h>


namespace imtcore
{


void CImtCoreBaseInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreBaseResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreStyleResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreBaseQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreBaseSdlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
