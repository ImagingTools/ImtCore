#include <imtcore/CImtCoreColGuiQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreColGuiQmlResources()
{
	Q_INIT_RESOURCE(imtcolguiqml);
}

namespace imtcore
{


void CImtCoreColGuiQmlInitializer::Initialize()
{
	DoInitImtCoreColGuiQmlResources();
}


} // namespace imtcore
