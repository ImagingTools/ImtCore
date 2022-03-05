#include <imtrest/CFileSender.h>


// Qt includes
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <iser/CJsonStringReadArchive.h>



#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtDebug>

// ACF includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <imod/CModelBase.h>



#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CJsonStringWriteArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TSmartPtr.h>

namespace imtrest
{


CFileSender::CFileSender(QObject *parent) : QObject(parent)
{

}

CFileSender::~CFileSender()
{

}

bool CFileSender::SendFile(QString query)
{
//    QQmlEngine* engine = qmlEngine(this);
//    if (engine != nullptr){
//        SetState("Loading");
//        QNetworkAccessManager* accessManager = engine->networkAccessManager();
//        qDebug() << "baseUrl" << engine->baseUrl();
//        QNetworkReply* reply = accessManager->post(QNetworkRequest(engine->baseUrl()), query.toUtf8());
//        connect(reply, &QNetworkReply::finished,
//                this, &CFileSender::replyFinished);
//        return true;
//    }

    return false;
}


void CFileSender::replyFinished()
{
//    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
//    if(reply){
//        QByteArray representationData = reply->readAll();
//        qDebug() << representationData;
////        Parse(representationData);
//        SetState("Ready");
//        reply->deleteLater();
//    }
}

//void CTreeItemModel::SetState(const QString &newState)
//{
//    if (m_state != newState){
//        m_state = newState;

//        emit stateChanged(m_state);
//    }
//}

} // namespace imtrest


