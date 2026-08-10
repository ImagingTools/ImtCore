#include <imtcore/CImtCoreDeskResourcesInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreChatDbResourcesInitializer.h>
#include <imtcore/CImtCoreDeskDbResourcesInitializer.h>
#include <imtcore/CImtCoreCommonDbResourcesInitializer.h>
#include <imtcore/CImtCoreGuiResourcesInitializer.h>
#include <imtcore/CImtCoreGuiLightResourcesInitializer.h>
#include <imtcore/CImtCoreGuiDarkResourcesInitializer.h>

namespace imtcore
{


void CImtCoreDeskResourcesInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreChatDbResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDeskDbResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreCommonDbResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreGuiResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreGuiLightResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreGuiDarkResourcesInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
