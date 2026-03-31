// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CChatServiceComp.h>

// Qt includes
#include <QtCore/QUuid>


namespace imtchat
{


// reimplemented (imtchat::IChatService)

QByteArray CChatServiceComp::SendMessage(
			const QByteArray& conversationId,
			const QString& content,
			const QByteArrayList& entityReferences,
			const QByteArrayList& attachmentIds)
{
	imtbase::IObjectCollection* messageCollectionPtr = m_messageCollectionCompPtr.GetPtr();
	if (messageCollectionPtr == nullptr){
		return QByteArray();
	}

	istd::IChangeableUniquePtr messagePtr(m_messageFactCompPtr->CreateInstance());
	if (messagePtr.IsEmpty()){
		return QByteArray();
	}

	IChatMessage* chatMessagePtr = dynamic_cast<IChatMessage*>(messagePtr.GetPtr());
	if (chatMessagePtr == nullptr){
		return QByteArray();
	}

	QByteArray newId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	chatMessagePtr->SetId(newId);
	chatMessagePtr->SetConversationId(conversationId);
	chatMessagePtr->SetContent(content);
	chatMessagePtr->SetStatus(IChatMessage::MS_SENT);
	chatMessagePtr->SetEntityReferences(entityReferences);
	chatMessagePtr->SetAttachmentIds(attachmentIds);

	QByteArray objectId = messageCollectionPtr->InsertNewObject(
				QByteArray(),
				QString(),
				QString(),
				messagePtr.GetPtr(),
				newId);

	if (objectId.isEmpty()){
		return QByteArray();
	}

	return objectId;
}


QByteArrayList CChatServiceComp::GetMessages(
			const QByteArray& conversationId,
			int offset,
			int limit) const
{
	Q_UNUSED(conversationId);

	const imtbase::IObjectCollection* messageCollectionPtr = m_messageCollectionCompPtr.GetPtr();
	if (messageCollectionPtr == nullptr){
		return QByteArrayList();
	}

	imtbase::ICollectionInfo::Ids ids = messageCollectionPtr->GetElementIds(offset, limit);

	QByteArrayList result;
	result.reserve(ids.size());
	for (const auto& id: ids){
		result.append(id);
	}

	return result;
}


QByteArray CChatServiceComp::CreateConversation(
			const QString& name,
			int conversationType,
			const QByteArrayList& participantIds)
{
	imtbase::IObjectCollection* conversationCollectionPtr = m_conversationCollectionCompPtr.GetPtr();
	if (conversationCollectionPtr == nullptr){
		return QByteArray();
	}

	istd::IChangeableUniquePtr conversationPtr(m_conversationFactCompPtr->CreateInstance());
	if (conversationPtr.IsEmpty()){
		return QByteArray();
	}

	IConversation* convPtr = dynamic_cast<IConversation*>(conversationPtr.GetPtr());
	if (convPtr == nullptr){
		return QByteArray();
	}

	QByteArray newId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	convPtr->SetId(newId);
	convPtr->SetName(name);
	convPtr->SetConversationType(static_cast<IConversation::ConversationType>(conversationType));
	convPtr->SetParticipantIds(participantIds);

	QByteArray objectId = conversationCollectionPtr->InsertNewObject(
				QByteArray(),
				name,
				QString(),
				conversationPtr.GetPtr(),
				newId);

	if (objectId.isEmpty()){
		return QByteArray();
	}

	return objectId;
}


QByteArrayList CChatServiceComp::GetConversations(int offset, int limit) const
{
	const imtbase::IObjectCollection* conversationCollectionPtr = m_conversationCollectionCompPtr.GetPtr();
	if (conversationCollectionPtr == nullptr){
		return QByteArrayList();
	}

	imtbase::ICollectionInfo::Ids ids = conversationCollectionPtr->GetElementIds(offset, limit);

	QByteArrayList result;
	result.reserve(ids.size());
	for (const auto& id: ids){
		result.append(id);
	}

	return result;
}


bool CChatServiceComp::MarkMessageRead(
			const QByteArray& conversationId,
			const QByteArray& messageId)
{
	Q_UNUSED(conversationId);

	imtbase::IObjectCollection* messageCollectionPtr = m_messageCollectionCompPtr.GetPtr();
	if (messageCollectionPtr == nullptr){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!messageCollectionPtr->GetObjectData(messageId, dataPtr)){
		return false;
	}

	IChatMessage* chatMessagePtr = dynamic_cast<IChatMessage*>(dataPtr.GetPtr());
	if (chatMessagePtr == nullptr){
		return false;
	}

	chatMessagePtr->SetStatus(IChatMessage::MS_READ);

	return messageCollectionPtr->SetObjectData(messageId, *chatMessagePtr);
}


} // namespace imtchat
