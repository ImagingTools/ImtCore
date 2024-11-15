#include <graphqlserver/CExternGraphQlPublisherComp.h>


// ImtCore includes
// #include <graphqlserver/CGqlRequest.h>



namespace graphqlserver
{



// public methods
bool CExternGraphQlPublisherComp::RegisterGqlPublisher(const QByteArray& commandId, IGqlPublisher& gqlPublisher, QString& errorMessage)
{
	m_gqlPublishers.insert(commandId, &gqlPublisher);

	return true;
}


bool CExternGraphQlPublisherComp::PublishData(const QByteArray& subscriptionId, const QJsonObject& subscriptionData)
{
	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		QByteArrayList keys = requestNetworks.networkRequests.keys();
		if (keys.contains(subscriptionId)){
			QJsonDocument document;
			document.setObject(subscriptionData);
			QByteArray data =  document.toJson(QJsonDocument::Compact);

			for (const QByteArray& id: keys){
				const imtrest::IRequest* networkRequestPtr = requestNetworks.networkRequests[id];
				if (networkRequestPtr != nullptr){
					SetData(id, "", data, *networkRequestPtr);
				}
			}
		}
	}

	return true;
}


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)
bool CExternGraphQlPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_gqlPublishers.contains(commandId);
}


bool CExternGraphQlPublisherComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (m_gqlPublishers.contains(commandId)){
		const imtgql::CGqlObject& gqlResultKeys = gqlRequest.GetFields();
		graphqlserver::ResultKeys resultKeys;
		CreateResultKeys(gqlResultKeys, resultKeys);
		if (m_gqlPublishers.value(commandId)->RegisterSubscription(subscriptionId, resultKeys, errorMessage)){
			m_gqlSubscriptions.insert(subscriptionId, m_gqlPublishers.value(commandId));
			return BaseClass::RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage);
		}
	}

	return false;
}


bool CExternGraphQlPublisherComp::UnRegisterSubscription(const QByteArray& subscriptionId)
{
	BaseClass::UnRegisterSubscription(subscriptionId);
	if (m_gqlSubscriptions.contains(subscriptionId)){
		QString errorMessage;
		graphqlserver::IGqlPublisher* externPublisherPtr = m_gqlSubscriptions.value(subscriptionId);
		externPublisherPtr->UnRegisterSubscription(subscriptionId, errorMessage);

		m_gqlSubscriptions.remove(subscriptionId);
	}

	return true;
}


} // namespace graphqlserver


