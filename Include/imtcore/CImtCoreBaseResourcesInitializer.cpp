#include <imtcore/CImtCoreBaseResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreBaseResources()
{
	Q_INIT_RESOURCE(imtbase);
}

namespace imtcore
{


void CImtCoreBaseResourcesInitializer::Initialize()
{
	DoInitImtCoreBaseResources();
}


} // namespace imtcore
