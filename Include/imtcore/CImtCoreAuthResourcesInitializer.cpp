#include <imtcore/CImtCoreAuthResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreAuthResources()
{
	Q_INIT_RESOURCE(imtauthdb);
	Q_INIT_RESOURCE(imtauthguilight);
	Q_INIT_RESOURCE(imtauthguidark);
}

namespace imtcore
{


void CImtCoreAuthResourcesInitializer::Initialize()
{
	DoInitImtCoreAuthResources();
}


} // namespace imtcore
