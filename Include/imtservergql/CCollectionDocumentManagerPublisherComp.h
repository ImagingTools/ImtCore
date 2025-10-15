#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


class CCollectionDocumentManagerPublisherComp:
	public CGqlPublisherCompBase,
	public imod::TSingleModelObserverBase<imtdoc::IDocumentManager>
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerPublisherComp)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "DummyCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (iomod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:


	template<class Representation>
	void PublishRepresentation(
		const QByteArray& commandId,
		const QByteArray& userId,
		const Representation& representation);

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};



// privte methods

template<class Representation>
void CCollectionDocumentManagerPublisherComp::PublishRepresentation(
	const QByteArray& commandId,
	const QByteArray& userId,
	const Representation& representation)
{
	QJsonObject jsonObject;
	if (!representation.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	for (const RequestNetworks& networkRequest : m_registeredSubscribers){
		if (!userId.isEmpty()){
			const imtgql::IGqlContext* contextPtr = networkRequest.gqlRequest.GetRequestContext();
			if (contextPtr != nullptr){
				const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
				if (userInfoPtr != nullptr){
					if (userInfoPtr->GetId() != userId){
						continue;
					}
				}
			}
		}

		if (commandId == networkRequest.gqlRequest.GetCommandId()){
			PublishData(commandId, data);
		}
	}
}


} // namespace imtservergql


