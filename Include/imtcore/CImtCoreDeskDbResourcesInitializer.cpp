#include <imtcore/CImtCoreDeskDbResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreDeskDbResources()
{
	Q_INIT_RESOURCE(imtdeskdb);
}

namespace imtcore
{


void CImtCoreDeskDbResourcesInitializer::Initialize()
{
	DoInitImtCoreDeskDbResources();
}


} // namespace imtcore
