#include <imtcore/CImtCoreGuiResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiResources()
{
	Q_INIT_RESOURCE(imtgui);
}

namespace imtcore
{


void CImtCoreGuiResourcesInitializer::Initialize()
{
	DoInitImtCoreGuiResources();
}


} // namespace imtcore
