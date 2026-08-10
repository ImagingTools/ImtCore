#include <imtcore/CImtCoreBaseSdlInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreBaseSdlResources()
{
	Q_INIT_RESOURCE(imtbaseSearchSdl);
	Q_INIT_RESOURCE(imtbaseCommandsSdl);
	Q_INIT_RESOURCE(imtbaseSettingsSdl);
	Q_INIT_RESOURCE(imtbaseImtBaseTypesSdl);
	Q_INIT_RESOURCE(imtbaseImtCollectionSdl);
	Q_INIT_RESOURCE(imtbaseFileSystemSdl);
	Q_INIT_RESOURCE(imtbaseDocumentCollectionFilterSdl);
	Q_INIT_RESOURCE(imtbaseComplexCollectionFilterSdl);
	Q_INIT_RESOURCE(imtbaseDocumentRevisionSdl);
	Q_INIT_RESOURCE(imtbaseCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtbaseUndoManagerSdl);
	Q_INIT_RESOURCE(imtbaseDocumentIdFilterSdl);
	Q_INIT_RESOURCE(imtbaseProgressManagerSdl);
	Q_INIT_RESOURCE(imtbaseCollectionImportSdl);
	Q_INIT_RESOURCE(imtbaseFilterableSelectSdl);
}

namespace imtcore
{


void CImtCoreBaseSdlInitializer::Initialize()
{
	DoInitImtCoreBaseSdlResources();
}


} // namespace imtcore
