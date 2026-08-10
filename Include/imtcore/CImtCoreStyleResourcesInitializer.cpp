#include <imtcore/CImtCoreStyleResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreStyleResources()
{
	Q_INIT_RESOURCE(imtstylecontrolsqml);
	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtstylelight);
	Q_INIT_RESOURCE(imtstyledark);
}

namespace imtcore
{


void CImtCoreStyleResourcesInitializer::Initialize()
{
	DoInitImtCoreStyleResources();
}


} // namespace imtcore
