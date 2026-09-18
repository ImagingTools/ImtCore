// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CNetworkEventInterceptor.h>


namespace imtqml
{


void CNetworkEventInterceptor::InterceptRequest(QNetworkReply* reply, QObject* gqlRequestPtr)
{
	m_replyOwners.insert(reply, gqlRequestPtr);
	connect(reply, &QNetworkReply::finished, this, &CNetworkEventInterceptor::OnReplyFinished);
	connect(reply, &QObject::destroyed, this, [this, reply](){
		m_replyOwners.remove(reply);
	});
}


// public Q_SLOTS

void CNetworkEventInterceptor::OnReplyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		QByteArray requestBody = reply->property("requestBody").toByteArray();

		QObject* gqlRequestPtr = m_replyOwners.take(reply).data();

		if (statusCode == 401){
			emit unauthorized(requestBody, gqlRequestPtr);
		}
		else if (statusCode == 403){
			emit forbidden(requestBody, gqlRequestPtr);
		}
	}
}


} // namespace imtqml

