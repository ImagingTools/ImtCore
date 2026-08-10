#include <imtcore/CAcfLocInitializer.h>


Q_ALWAYS_INLINE static void DoInitAcfLocResources()
{
	Q_INIT_RESOURCE(AcfLoc);
	Q_INIT_RESOURCE(AcfSlnLoc);
}

namespace imtcore
{


void CAcfLocInitializer::Initialize()
{
	DoInitAcfLocResources();
}


} // namespace imtcore
