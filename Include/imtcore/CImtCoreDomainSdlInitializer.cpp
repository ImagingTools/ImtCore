#include <imtcore/CImtCoreDomainSdlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreDomainSdlResources()
{
	Q_INIT_RESOURCE(imtappApplicationSdl);
	Q_INIT_RESOURCE(imtcolorLabSdl);
	Q_INIT_RESOURCE(imtcolorRgbColorHexSdl);
	Q_INIT_RESOURCE(imt2dGeometrySdl);
	Q_INIT_RESOURCE(imt2dPaintPrimitivesSdl);
	Q_INIT_RESOURCE(imt2dPaintSystemSdl);
}

namespace imtcore
{


void CImtCoreDomainSdlInitializer::Initialize()
{
	DoInitImtCoreDomainSdlResources();
}


} // namespace imtcore
