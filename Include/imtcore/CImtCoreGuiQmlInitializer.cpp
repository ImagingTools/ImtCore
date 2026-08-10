#include <imtcore/CImtCoreGuiQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiQmlResources()
{
	Q_INIT_RESOURCE(imtguiqml);
}

namespace imtcore
{


void CImtCoreGuiQmlInitializer::Initialize()
{
	DoInitImtCoreGuiQmlResources();
}


} // namespace imtcore
