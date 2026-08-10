#include <imtcore/CImtCoreThemeInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreGuiThemeInitializer.h>
#include <imtcore/CImtCoreAuthThemeInitializer.h>

namespace imtcore
{


void CImtCoreThemeInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreGuiThemeInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreAuthThemeInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
