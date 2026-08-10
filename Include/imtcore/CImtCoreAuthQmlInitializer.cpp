#include <imtcore/CImtCoreAuthQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreAuthQmlResources()
{
	Q_INIT_RESOURCE(imtauthguiqml);
}

namespace imtcore
{


void CImtCoreAuthQmlInitializer::Initialize()
{
	DoInitImtCoreAuthQmlResources();
}


} // namespace imtcore
