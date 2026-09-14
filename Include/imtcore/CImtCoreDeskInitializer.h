#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitDeskSqlResources()
{
	Q_INIT_RESOURCE(imtchatdb);
	Q_INIT_RESOURCE(imtdeskdb);
}


Q_ALWAYS_INLINE static void ImtCoreInitDeskQmlResources()
{
	Q_INIT_RESOURCE(imtdeskguiqml);

	Q_INIT_RESOURCE(imtdeskImtDeskSdl);
	Q_INIT_RESOURCE(imtdeskTicketCollectionDocumentServiceSdl);
}
