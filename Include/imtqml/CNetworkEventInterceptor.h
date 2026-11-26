// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPointer>
#include <QtCore/QVariantMap>
#include <QtNetwork/QNetworkReply>


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

	void InterceptRequest(QNetworkReply* reply, QObject* gqlRequestPtr);

public Q_SLOTS:
	void OnReplyFinished();

signals:
	/// \param requestHeaders The headers its sender gave the request, without those of the session; it is sent again with them.
	void unauthorized(QString requestBody, QObject* request, QVariantMap requestHeaders, bool credentialsChanged);
	void forbidden(QString requestBody, QObject* request, bool credentialsChanged);

private:
	QHash<QNetworkReply*, QPointer<QObject>> m_replyOwners;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CNetworkEventInterceptor*)

