// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CChatServiceComp.h>

// Qt includes
#include <QtCore/QUuid>


namespace imtchat
{


// reimplemented (imtchat::IChatService)

QByteArray CChatServiceComp::CreateConversation(
			const QString& name,
			int conversationType,
			const QByteArrayList& participantIds)
{
	imtbase::IObjectCollection* conversationCollectionPtr = m_conversationCollectionCompPtr.GetPtr();
	if (conversationCollectionPtr == nullptr){
		return QByteArray();
	}

	istd::IChangeableUniquePtr conversationPtr(m_conversationFactCompPtr.CreateInstance());
	if (!conversationPtr.IsValid()){
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


} // namespace imtchat
