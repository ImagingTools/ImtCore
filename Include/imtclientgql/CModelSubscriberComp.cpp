#include "imtclientgql/CModelSubscriberComp.h"


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CModelSubscriberComp::OnResponseReceived(const QByteArray & subscriptionId, const QByteArray & subscriptionData)
{
	qDebug() << "CModelSubscriberComp OnResponseReceived" << subscriptionId << subscriptionData;
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::NotifierInfo notifierInfo;

	QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
	QJsonObject subscriptionObject = document.object();

	if (subscriptionId == m_subscriptionId){
		if (subscriptionObject.contains("id")){
			notifierInfo.elementId = subscriptionObject.value("id").toString().toUtf8();
			changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ALL_CHANGED, QVariant::fromValue(notifierInfo));
		}

		if (subscriptionObject.contains("token")){
			QByteArray token = subscriptionObject.value("token").toString().toUtf8();
			changeSet.SetChangeInfo("token", token);
		}
	}

	if (m_changeableModelCompPtr.IsValid()){
		qDebug() << "notifier";

		istd::CChangeNotifier notifier(m_changeableModelCompPtr.GetPtr(), &changeSet);
	}
}



} // namespace gmgaws


