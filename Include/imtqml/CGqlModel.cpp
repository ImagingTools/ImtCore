#include <imtqml/CGqlModel.h>


// Qt includes
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <iser/CJsonStringReadArchive.h>



namespace imtqml
{


QString CGqlModel::s_accessToken = QString();

CGqlModel::CGqlModel(QObject *parent)
	: imtbase::CTreeItemModel(parent)
{

}

CGqlModel::~CGqlModel()
{

}

bool CGqlModel::SetGqlQuery(QString query)
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

		if (!s_accessToken.isEmpty()){
			networkRequest.setRawHeader("X-authentication-token", s_accessToken.toUtf8());
		}

		QNetworkReply* reply = accessManager->post(networkRequest, query.toUtf8());
		connect(reply, &QNetworkReply::finished, this, &CGqlModel::replyFinished);

		return true;
	}

	return false;
}


void CGqlModel::replyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		qDebug() << representationData;
		Parse(representationData);
		SetState("Ready");
		reply->deleteLater();
	}
}


void CGqlModel::SetGlobalAccessToken(const QString &accessToken)
{
	if (s_accessToken != accessToken){
		s_accessToken = accessToken;
	}
}


} // namespace imtqml


