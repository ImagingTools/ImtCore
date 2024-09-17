#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtqml
{


class CGqlModel: public imtbase::CTreeItemModel
{
	Q_OBJECT

public:
	typedef imtbase::CTreeItemModel BaseClass;

	Q_INVOKABLE inline bool setGqlQuery(QString query){ return SetGqlQuery(query); }

	explicit CGqlModel(QObject* parent = nullptr);
	~CGqlModel();

public Q_SLOTS:
	bool SetGqlQuery(QString query);
	void replyFinished();
	void errorOccurred(QNetworkReply::NetworkError code);

	static void SetGlobalAccessToken(const QString& accessToken);

public:
	static QString s_accessToken;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlModel*)


