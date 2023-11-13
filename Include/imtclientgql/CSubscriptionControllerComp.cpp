#include "CSubscriptionControllerComp.h"


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/ICollectionInfo.h>


namespace imtclientgql
{


// protected methods

void CSubscriptionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, *m_commandAttrPtr);

		imtgql::CGqlObject subscriptionField("data");
		subscriptionField.InsertField("id");
		gqlRequest.AddField(subscriptionField);

		m_subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
	}
}


void CSubscriptionControllerComp::OnResponseReceived(
			const QByteArray & subscriptionId,
			const QByteArray & subscriptionData)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::NotifierInfo notifierInfo;

	QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
	QJsonObject subscriptionObject = document.object();

	if (subscriptionId == m_subscriptionId){
		notifierInfo.elementId = subscriptionObject.value("id").toString().toUtf8();
		changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ALL_CHANGED, QVariant::fromValue(notifierInfo));
	}

	if (m_changeableModelCompPtr.IsValid()){
		istd::CChangeNotifier notifier(m_changeableModelCompPtr.GetPtr(), &changeSet);
	}
}


void CSubscriptionControllerComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


} // namespace gmgaws


