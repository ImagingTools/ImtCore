#include <imtcore/CImtCoreSdlInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreBaseSdlInitializer.h>
#include <imtcore/CImtCoreAuthSdlInitializer.h>
#include <imtcore/CImtCoreDomainSdlInitializer.h>

namespace imtcore
{


void CImtCoreSdlInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreBaseSdlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthSdlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDomainSdlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
