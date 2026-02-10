// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlRequest.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>

// ImtCore includes
#include <imtqml/CGqlModel.h>
#include <imtqml/CNetworkEventInterceptor.h>


namespace imtqml
{


CGqlRequest::CGqlRequest(QObject *parent)
	:BaseClass(parent)
{
}


CGqlRequest::~CGqlRequest()
{
}


bool CGqlRequest::SetGqlQuery(QString query, QVariantMap headers)
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

		if (!CGqlModel::s_accessToken.isEmpty()){
			networkRequest.setRawHeader("x-authentication-token", CGqlModel::s_accessToken.toUtf8());
			QStringList keys = headers.keys();
			for (QString& key: keys){
				networkRequest.setRawHeader(key.toLatin1(), headers.value(key).toByteArray());
			}
		}

		QString message = QString("Post to url '%1' query '%2'").arg(requestUrl.toString()).arg(query);
		qDebug() << message;

		QNetworkReply* reply = accessManager->post(networkRequest, query.toUtf8());
		reply->setProperty("requestBody", query.toUtf8());

		connect(reply, &QNetworkReply::finished, this, &CGqlRequest::replyFinished);
		connect(reply, &QNetworkReply::errorOccurred, this, &CGqlRequest::errorOccurred);
		connect(reply, &QNetworkReply::sslErrors, this, &CGqlRequest::onSslErrors);
		CNetworkEventInterceptor::Instance()->InterceptRequest(reply, this);

		return true;
	}

	return false;
}


void CGqlRequest::replyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (statusCode == 401){
			SetState("Unauthorized");
		}
		else if (statusCode == 403){
			SetState("Forbidden");
		}
		else if (statusCode == 200){
			m_json = reply->readAll();

			if (m_json.isEmpty()){
				SetState("Error");
			}
			else{
				if (m_state == "Ready"){
					SetState("Loading");
				}

				SetState("Ready");
			}
		}

		reply->deleteLater();
	}
}


void CGqlRequest::errorOccurred(QNetworkReply::NetworkError /*code*/)
{
	QNetworkReply* networkReplyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (networkReplyPtr != nullptr){
		qDebug() << networkReplyPtr->errorString();

		networkReplyPtr->deleteLater();
	}
}

void CGqlRequest::onSslErrors(const QList<QSslError>& /*errors*/)
{
	QNetworkReply* networkReplyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (networkReplyPtr != nullptr){
		networkReplyPtr->ignoreSslErrors();
	}
}


// private methods

const QString &CGqlRequest::State() const
{
	return m_state;
}


void CGqlRequest::SetState(const QString& newState)
{
	if (m_state != newState){
		m_state = newState;

		Q_EMIT stateChanged(m_state);
	}
}


QString CGqlRequest::GetJson() const
{
	return m_json;
}


void CGqlRequest::SetJson(const QString& json)
{
	if (m_json != json){
		m_json = json;

		Q_EMIT jsonChanged(m_json);
	}
}


} // namespace imtqml


