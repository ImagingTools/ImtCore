// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CGqlModel.h>


// Qt includes
#include <QtCore/QVariantMap>
#include <QtNetwork/QNetworkRequest>

// ImtCore includes
#include <imtbase/imtbase.h>


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
		if (statusCode != 401 && statusCode != 403){
			return;
		}
		const QNetworkRequest request = reply->request();
		const bool credentialsChanged = request.rawHeader(imtbase::s_authenticationTokenHeaderId) != CGqlModel::GetGlobalAccessToken().toUtf8();

		if (statusCode == 401){
			// The model sets the session headers of every request itself
			QVariantMap requestHeaders;
			for (const QByteArray& headerId : request.rawHeaderList()){
				const QByteArray lowerHeaderId = headerId.toLower();
				if (lowerHeaderId != imtbase::s_authenticationTokenHeaderId && lowerHeaderId != "productid" && lowerHeaderId != "content-type"){
					requestHeaders.insert(QString::fromUtf8(headerId), QString::fromUtf8(request.rawHeader(headerId)));
				}
			}

			emit unauthorized(requestBody, gqlRequestPtr, requestHeaders, credentialsChanged);
		}
		else if (statusCode == 403){
			emit forbidden(requestBody, gqlRequestPtr, credentialsChanged);
		}
	}
}


} // namespace imtqml

