#include <imtqml/CGqlModel.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>


namespace imtqml
{


QString CGqlModel::s_accessToken = QString();


CGqlModel::CGqlModel(QObject *parent)
	:BaseClass(parent)
{
}


CGqlModel::~CGqlModel()
{
}


bool CGqlModel::SetGqlQuery(QString query, QVariantMap headers)
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
			networkRequest.setRawHeader("X-authentication-token", s_accessToken.toUtf8());
			QStringList keys = headers.keys();
			for (QString& key: keys){
				networkRequest.setRawHeader(key.toLatin1(), headers.value(key).toByteArray());
			}
		}

		QString message = QString("Post to url '%1' query '%2'").arg(requestUrl.toString()).arg(query);
		qDebug() << message;

		QNetworkReply* reply = accessManager->post(networkRequest, query.toUtf8());
		connect(reply, &QNetworkReply::finished, this, &CGqlModel::replyFinished);
		connect(reply, &QNetworkReply::errorOccurred, this, &CGqlModel::errorOccurred);

		return true;
	}

	return false;
}


void CGqlModel::replyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		bool result = CreateFromJson(representationData);
		if (result){
			if (State() == "Ready"){
				SetState("Loading");
			}

			SetState("Ready");
		}
		else{
			SetState("Error");
		}
		reply->deleteLater();
	}
}


void CGqlModel::errorOccurred(QNetworkReply::NetworkError /*code*/)
{
	QNetworkReply* networkReplyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (networkReplyPtr != nullptr){
		qDebug() << networkReplyPtr->errorString();

		networkReplyPtr->deleteLater();
	}
}


void CGqlModel::SetGlobalAccessToken(const QString &accessToken)
{
	if (s_accessToken != accessToken){
		s_accessToken = accessToken;
	}
}


} // namespace imtqml


