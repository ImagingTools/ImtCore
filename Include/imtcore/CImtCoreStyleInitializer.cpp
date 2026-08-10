#include <imtcore/CImtCoreStyleInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreStyleTypeInitializer.h>
#include <imtcore/CImtCoreFusionBaseStyleInitializer.h>
#include <imtcore/CImtCoreLightThemePropertyInitializer.h>


namespace imtcore
{


void CImtCoreStyleInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreStyleTypeInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreFusionBaseStyleInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreLightThemePropertyInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
