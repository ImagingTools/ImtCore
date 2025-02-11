#include <imtqml/CNetworkEventInterceptor.h>


namespace imtqml
{


void CNetworkEventInterceptor::InterceptRequest(QNetworkReply* reply, CGqlRequest* gqlRequestPtr)
{
	m_replyOwners.insert(reply, gqlRequestPtr);
	connect(reply, &QNetworkReply::finished, this, &CNetworkEventInterceptor::OnReplyFinished);
}


// public Q_SLOTS

void CNetworkEventInterceptor::OnReplyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		QByteArray requestBody = reply->property("requestBody").toByteArray();

		qDebug() << "OnReplyFinished" << statusCode << requestBody;

		CGqlRequest* gqlRequestPtr = nullptr;
		if (m_replyOwners.contains(reply)){
			gqlRequestPtr = m_replyOwners.take(reply);
		}

		if (statusCode == 401){
			emit unauthorized(requestBody, gqlRequestPtr);
		}
		else if (statusCode == 403){
			emit forbidden(requestBody, gqlRequestPtr);
		}
	}
}


} // namespace imtqml

