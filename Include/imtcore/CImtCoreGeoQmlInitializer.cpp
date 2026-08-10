#include <imtcore/CImtCoreGeoQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGeoQmlResources()
{
	Q_INIT_RESOURCE(imtgeoguiqml);
}

namespace imtcore
{


void CImtCoreGeoQmlInitializer::Initialize()
{
	DoInitImtCoreGeoQmlResources();
}


} // namespace imtcore
