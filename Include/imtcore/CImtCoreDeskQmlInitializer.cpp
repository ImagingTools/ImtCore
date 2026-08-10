#include <imtcore/CImtCoreDeskQmlInitializer.h>

// ImtCore includes
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreGuiQmlInitializer.h>
#include <imtcore/CImtCoreColGuiQmlInitializer.h>
#include <imtcore/CImtCoreDocGuiQmlInitializer.h>
#include <imtcore/CImtCoreGuiGqlQmlInitializer.h>

namespace imtcore
{


void CImtCoreDeskQmlInitializer::Initialize()
{
	CCascadedApplicationInitializer cascadedInitializer;

	cascadedInitializer.AddInitializer(new CImtCoreGuiQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreColGuiQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreDocGuiQmlInitializer());
	cascadedInitializer.AddInitializer(new CImtCoreGuiGqlQmlInitializer());

	cascadedInitializer.Initialize();
}


} // namespace imtcore
