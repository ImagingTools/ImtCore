#include <imtcore/CImtCoreLocalizationInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>

// ImtCore includes
#include <imtcore/CAcfLocInitializer.h>


Q_ALWAYS_INLINE static void DoInitImtCoreLocalizationResources()
{
	imtcore::CAcfLocInitializer acfLocInitializer;
	acfLocInitializer.Initialize();

	Q_INIT_RESOURCE(ImtCoreLoc);
}

namespace imtcore
{


void InitializeImtCoreLocalization()
{
	DoInitImtCoreLocalizationResources();
}


} // namespace imtcore
