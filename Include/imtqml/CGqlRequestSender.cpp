#include <imtqml/CGqlRequestSender.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>


namespace imtqml
{


QString CGqlRequestSender::s_accessToken = QString();


// public methods

void CGqlRequestSender::SetGlobalAccessToken(const QString &accessToken)
{
	if (s_accessToken != accessToken){
		s_accessToken = accessToken;
	}
}


bool CGqlRequestSender::SetGqlQuery(QString query)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();

		QUrl requestUrl = engine->baseUrl();

		QString urlPath = requestUrl.path();
		urlPath.append("/graphql");
		requestUrl.setPath(urlPath);

		QNetworkRequest networkRequest = QNetworkRequest(requestUrl);
		networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

		if (!s_accessToken.isEmpty()){
			networkRequest.setRawHeader("x-authentication-token", s_accessToken.toUtf8());
		}

		QString message = QString("Post to url '%1' query '%2'").arg(requestUrl.toString()).arg(query);
		qDebug() << message;

		QNetworkReply* reply = accessManager->post(networkRequest, query.toUtf8());
		connect(reply, &QNetworkReply::finished, this, &CGqlRequestSender::replyFinished);
		connect(reply, &QNetworkReply::errorOccurred, this, &CGqlRequestSender::errorOccurred);

		return true;
	}

	return false;
}


void CGqlRequestSender::OnReplyFinished(const QByteArray& /*data*/)
{
}


void CGqlRequestSender::replyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();

		OnReplyFinished(representationData);

		reply->deleteLater();
	}
}


void CGqlRequestSender::errorOccurred(QNetworkReply::NetworkError /*code*/)
{
}


// private methods

const QString &CGqlRequestSender::State() const
{
	return m_state;
}


void CGqlRequestSender::SetState(const QString& newState)
{
	if (m_state != newState){
		m_state = newState;

		Q_EMIT stateChanged(m_state);
	}
}


} // namespace imtqml


