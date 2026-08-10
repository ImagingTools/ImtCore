#include <imtcore/CImtCoreResourcesInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreAuthResourcesInitializer.h>
#include <imtcore/CImtCoreLicResourcesInitializer.h>
#include <imtcore/CImtCoreDeskResourcesInitializer.h>
#include <imtcore/CImtCoreBaseResourcesInitializer.h>
#include <imtcore/CImtCoreStyleResourcesInitializer.h>

namespace imtcore
{


void CImtCoreResourcesInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreAuthResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreLicResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDeskResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreBaseResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreStyleResourcesInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
