// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlRequest.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>

// ImtCore includes
#include <imtbase/imtbase.h>
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

		QStringList keys = headers.keys();
		for (QString& key: keys){
			networkRequest.setRawHeader(key.toLatin1(), headers.value(key).toByteArray());
		}

		if (!CGqlModel::s_productId.isEmpty()){
			if (!networkRequest.hasRawHeader("productId")){
				networkRequest.setRawHeader("productId", CGqlModel::s_productId.toUtf8());
			}
		}

		if (!CGqlModel::s_accessToken.isEmpty()){
			if (!networkRequest.hasRawHeader(imtbase::s_authenticationTokenHeaderId)){
				networkRequest.setRawHeader(imtbase::s_authenticationTokenHeaderId, CGqlModel::s_accessToken.toUtf8());
			}
		}

		QString message = QStringLiteral("Post to url '%1' query '%2'").arg(requestUrl.toString(), query);
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
		else{
			// Every outcome has to leave "Loading". Previously anything that was not
			// 200/401/403 - a 5xx, a 404, or a transport failure with no HTTP status
			// at all - left the request in "Loading" forever, so callers waiting on
			// the state (and single-flight guards keyed on it) never completed.
			qDebug() << "CGqlRequest: request failed, HTTP status" << statusCode
					 << "error" << reply->errorString();

			SetState("Error");
		}

		reply->deleteLater();
	}
}


void CGqlRequest::errorOccurred(QNetworkReply::NetworkError /*code*/)
{
	QNetworkReply* networkReplyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (networkReplyPtr != nullptr){
		// Deliberately no deleteLater() here: QNetworkReply always emits finished()
		// after errorOccurred(), and replyFinished() owns both the terminal state and
		// the reply's lifetime. Deleting in two places left the failure path setting
		// no state at all.
		qDebug() << networkReplyPtr->errorString();
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


