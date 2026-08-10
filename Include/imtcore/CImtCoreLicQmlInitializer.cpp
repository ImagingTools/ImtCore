#include <imtcore/CImtCoreLicQmlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreLicQmlResources()
{
	Q_INIT_RESOURCE(imtlicguiqml);
}

namespace imtcore
{


void CImtCoreLicQmlInitializer::Initialize()
{
	DoInitImtCoreLicQmlResources();
}


} // namespace imtcore
