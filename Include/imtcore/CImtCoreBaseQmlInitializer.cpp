#include <imtcore/CImtCoreBaseQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreBaseQmlResources()
{
	Q_INIT_RESOURCE(imtcontrolsqml);
}

namespace imtcore
{


void CImtCoreBaseQmlInitializer::Initialize()
{
	DoInitImtCoreBaseQmlResources();
}


} // namespace imtcore
