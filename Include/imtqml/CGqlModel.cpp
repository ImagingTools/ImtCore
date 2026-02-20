// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlModel.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>


namespace imtqml
{


QString CGqlModel::s_accessToken = QString();
QString CGqlModel::s_refreshToken = QString();


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
			networkRequest.setRawHeader("x-authentication-token", s_accessToken.toUtf8());
			QStringList keys = headers.keys();
			for (QString& key: keys){
				networkRequest.setRawHeader(key.toLatin1(), headers.value(key).toByteArray());
			}
		}

		QString message = QString("Post to url '%1' query '%2'").arg(requestUrl.toString()).arg(query);
		qDebug() << message;

		QNetworkReply* replyPtr = accessManager->post(networkRequest, query.toUtf8());
		if (replyPtr != nullptr){
			replyPtr->setProperty("requestBody", query.toUtf8());
			
			replyPtr->ignoreSslErrors();
			connect(replyPtr, &QNetworkReply::finished, this, &CGqlModel::replyFinished);
			connect(replyPtr, &QNetworkReply::errorOccurred, this, &CGqlModel::errorOccurred);
		}

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
		qDebug() << "CGqlModel errorOccurred: " << networkReplyPtr->errorString();

		networkReplyPtr->deleteLater();
	}
}


void CGqlModel::SetGlobalAccessToken(const QString &accessToken)
{
	if (s_accessToken != accessToken){
		s_accessToken = accessToken;
	}
}


void CGqlModel::SetRefreshToken(const QString& refreshToken)
{
	if (s_refreshToken != refreshToken){
		s_refreshToken = refreshToken;
	}
}


} // namespace imtqml

