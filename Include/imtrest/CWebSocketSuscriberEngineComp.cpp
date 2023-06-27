#include <imtrest/CWebSocketSuscriberEngineComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include <imtrest/CSubscriberBase.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>


namespace imtrest
{


// public methods

CWebSocketSubscriberEngineComp::CWebSocketSubscriberEngineComp()
{
}


// reimplemented (ISuscriberEngine)

ISubscriber* CWebSocketSubscriberEngineComp::RegisterSubscriber(
			QObject* socketPtr,
			const IRequestServlet& requestHandler,
			const IProtocolEngine& engine)
{
	CSubscriberBase* subscriberPtr = new CSubscriberBase(*socketPtr, requestHandler, engine);

	m_subscribers.PushBack(subscriberPtr);

	return subscriberPtr;
}


void CWebSocketSubscriberEngineComp::UnRegisterSubscriber(QObject *socketObjectPtr)
{
	for (int i = 0; i < m_subscribers.GetCount(); ++i){
		ISubscriber* subscriberPtr = m_subscribers.GetAt(i);
		Q_ASSERT(subscriberPtr != nullptr);

		if (&subscriberPtr->GetSocketObject() == socketObjectPtr){
			m_subscribers.RemoveAt(i);

			break;
		}
	}
}


const ISubscriber* CWebSocketSubscriberEngineComp::GetSubscriber(const IRequest *requestPtr) const
{
	QObject* socketObjectPtr = &requestPtr->GetSocketObject();
	for (int i = 0; i < m_subscribers.GetCount(); ++i){
		ISubscriber* subscriberPtr = m_subscribers.GetAt(i);
		Q_ASSERT(subscriberPtr != nullptr);

		if (&subscriberPtr->GetSocketObject() == socketObjectPtr){
			return m_subscribers.GetAt(i);

		}
	}
	return nullptr;
}


int CWebSocketSubscriberEngineComp::GetSubscriberCount() const
{
	return m_subscribers.GetCount();
}


const ISubscriber* CWebSocketSubscriberEngineComp::GetSubscriber(int index) const
{
	return m_subscribers.GetAt(index);
}


void CWebSocketSubscriberEngineComp::OnModelUpdate(QByteArray modelId, const istd::IChangeable::ChangeSet& changeSet) const
{
	imtbase::CTreeItemModel treeItemModel;
	QByteArray body;
	treeItemModel.SetData("modelId",modelId);
	treeItemModel.SetData("description",changeSet.GetDescription());
	imtbase::CTreeItemModel *treeModelPtr = treeItemModel.AddTreeModel("data");
	istd::IChangeable::ChangeInfoMap infoMap = changeSet.GetChangeInfoMap();

	for (QByteArray key: infoMap.keys()){
		treeModelPtr->SetData(key,infoMap.value(key));
	}

	{
		iser::CJsonMemWriteArchive archive(body);
		treeItemModel.Serialize(archive);
	}

	for (int i = 0; i < m_subscribers.GetCount(); ++i){
		ISubscriber* subscriberPtr = m_subscribers.GetAt(i);
		Q_ASSERT(subscriberPtr != nullptr);

		IRequest *irequestPtr = subscriberPtr->GetProtocolEngine().CreateRequestForSend(
					&subscriberPtr->GetSocketObject(),
					subscriberPtr->GetRequestHandler(),0,body,QByteArray());

		subscriberPtr->GetProtocolEngine().GetSender().SendRequest(*irequestPtr);
	}
}


} // namespace imtrest


