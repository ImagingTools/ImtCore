#include <imtcore/CImtCoreGuiDarkResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiDarkResources()
{
	Q_INIT_RESOURCE(imtguidark);
}

namespace imtcore
{


void CImtCoreGuiDarkResourcesInitializer::Initialize()
{
	DoInitImtCoreGuiDarkResources();
}


} // namespace imtcore
