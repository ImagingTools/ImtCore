#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>



namespace imtgql
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/
class CGqlModel: public imtbase::CTreeItemModel
{
	Q_OBJECT

public:
	explicit CGqlModel(QObject* parent = nullptr);
	~CGqlModel();

public Q_SLOTS:
	bool SetGqlQuery(QString query);
	void replyFinished();
};


} // namespace imtgql

Q_DECLARE_METATYPE(imtgql::CGqlModel*)
