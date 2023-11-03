#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>



namespace imtqml
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/
class CGqlModel: public imtbase::CTreeItemModel
{
	Q_OBJECT

public:
	typedef imtbase::CTreeItemModel BaseClass;

	explicit CGqlModel(QObject* parent = nullptr);
	~CGqlModel();

public Q_SLOTS:
	bool SetGqlQuery(QString query);
	void replyFinished();
	static void SetGlobalAccessToken(const QString& accessToken);

private:
	static QString s_accessToken;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlModel*)


