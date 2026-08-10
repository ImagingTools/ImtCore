#include <imtcore/CImtCoreLicResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreLicResources()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);
}

namespace imtcore
{


void CImtCoreLicResourcesInitializer::Initialize()
{
	DoInitImtCoreLicResources();
}


} // namespace imtcore
