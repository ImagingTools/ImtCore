#include <imtcore/CImtCoreCommonDbResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreCommonDbResources()
{
	Q_INIT_RESOURCE(imtdb);
}

namespace imtcore
{


void CImtCoreCommonDbResourcesInitializer::Initialize()
{
	DoInitImtCoreCommonDbResources();
}


} // namespace imtcore
