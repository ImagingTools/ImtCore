#include <imtcore/CImtCoreGuiGqlQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiGqlQmlResources()
{
	Q_INIT_RESOURCE(imtguigqlqml);
}

namespace imtcore
{


void CImtCoreGuiGqlQmlInitializer::Initialize()
{
	DoInitImtCoreGuiGqlQmlResources();
}


} // namespace imtcore
