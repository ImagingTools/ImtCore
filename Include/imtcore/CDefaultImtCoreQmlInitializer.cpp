#include <imtcore/CDefaultImtCoreQmlInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>
#include <imtcore/CImtCoreBaseInitializer.h>
#include <imtcore/CImtCoreAuthInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtcore/CImtCoreLicInitializer.h>


namespace imtcore
{


void CDefaultImtCoreQmlInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreLocalizationInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreStyleInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreBaseInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDeskInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreLicInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
