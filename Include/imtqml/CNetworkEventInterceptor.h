#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtqml/CGqlRequest.h>


namespace imtqml
{


class CNetworkEventInterceptor: public QObject
{
	Q_OBJECT

public:
	static CNetworkEventInterceptor* Instance()
	{
		static CNetworkEventInterceptor instance;
		return &instance;
	}

	void InterceptRequest(QNetworkReply* reply, CGqlRequest* gqlRequestPtr);

public Q_SLOTS:
	void OnReplyFinished();

signals:
	void unauthorized(QString requestBody, CGqlRequest* request);
	void forbidden(QString requestBody, CGqlRequest* request);

private:
	QHash<QNetworkReply*, CGqlRequest*> m_replyOwners;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CNetworkEventInterceptor*)

