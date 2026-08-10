#include <imtcore/CImtCoreDeskResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreDeskResources()
{
	Q_INIT_RESOURCE(imtchatdb);
	Q_INIT_RESOURCE(imtdeskdb);
	Q_INIT_RESOURCE(imtdb);

	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtguilight);
	Q_INIT_RESOURCE(imtguidark);
}

namespace imtcore
{


void CImtCoreDeskResourcesInitializer::Initialize()
{
	DoInitImtCoreDeskResources();
}


} // namespace imtcore
