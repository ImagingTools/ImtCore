#include <imtcore/CImtCoreChatDbResourcesInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreChatDbResources()
{
	Q_INIT_RESOURCE(imtchatdb);
}

namespace imtcore
{


void CImtCoreChatDbResourcesInitializer::Initialize()
{
	DoInitImtCoreChatDbResources();
}


} // namespace imtcore
