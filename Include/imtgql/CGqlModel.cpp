#include <imtgql/CGqlModel.h>


// Qt includes
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <iser/CJsonStringReadArchive.h>



namespace imtgql
{


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
		qDebug() << "baseUrl" << engine->baseUrl();
		QNetworkReply* reply = accessManager->post(QNetworkRequest(engine->baseUrl()), query.toUtf8());
		connect(reply, &QNetworkReply::finished,
				this, &CGqlModel::replyFinished);
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


} // namespace imtgql


