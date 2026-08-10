#include <imtcore/CImtCoreQmlInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreBaseQmlInitializer.h>
#include <imtcore/CImtCoreDeskQmlInitializer.h>
#include <imtcore/CImtCoreAuthQmlInitializer.h>
#include <imtcore/CImtCoreLicQmlInitializer.h>

namespace imtcore
{


void CImtCoreQmlInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreBaseQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDeskQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreLicQmlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
