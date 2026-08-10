#include <imtcore/CImtCoreGuiLightResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiLightResources()
{
	Q_INIT_RESOURCE(imtguilight);
}

namespace imtcore
{


void CImtCoreGuiLightResourcesInitializer::Initialize()
{
	DoInitImtCoreGuiLightResources();
}


} // namespace imtcore
