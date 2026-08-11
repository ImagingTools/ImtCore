#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitBaseResourcesGlobal()
{
	Q_INIT_RESOURCE(imtbase);

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

Q_ALWAYS_INLINE static void ImtCoreInitBaseUiResourcesGlobal()
{
	Q_INIT_RESOURCE(imtstylecontrolsqml);

	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtstylelight);
	Q_INIT_RESOURCE(imtstyledark);

	Q_INIT_RESOURCE(imtcontrolsqml);
}

namespace imtcore
{


inline void InitializeImtCoreBase()
{
	ImtCoreInitBaseResourcesGlobal();
}

inline void InitializeImtCoreBaseUi()
{
	ImtCoreInitBaseUiResourcesGlobal();
}


} // namespace imtcore
