#include <imtcore/CImtCoreDeskQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreDeskQmlResources()
{
	Q_INIT_RESOURCE(imtguiqml);
	Q_INIT_RESOURCE(imtcolguiqml);
	Q_INIT_RESOURCE(imtdocguiqml);
	Q_INIT_RESOURCE(imtguigqlqml);
}

namespace imtcore
{


void CImtCoreDeskQmlInitializer::Initialize()
{
	DoInitImtCoreDeskQmlResources();
}


} // namespace imtcore
