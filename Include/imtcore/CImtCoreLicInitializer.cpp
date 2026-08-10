#include <imtcore/CImtCoreLicInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreLicResourcesInitializer.h>
#include <imtcore/CImtCoreLicQmlInitializer.h>


namespace imtcore
{


void CImtCoreLicInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreLicResourcesInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreLicQmlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
