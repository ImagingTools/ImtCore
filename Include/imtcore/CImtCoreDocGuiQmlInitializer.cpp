#include <imtcore/CImtCoreDocGuiQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreDocGuiQmlResources()
{
	Q_INIT_RESOURCE(imtdocguiqml);
}

namespace imtcore
{


void CImtCoreDocGuiQmlInitializer::Initialize()
{
	DoInitImtCoreDocGuiQmlResources();
}


} // namespace imtcore
