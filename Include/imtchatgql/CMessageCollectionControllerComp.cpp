// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatgql/CMessageCollectionControllerComp.h>

// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtchat/IMessage.h>


namespace imtchatgql
{


bool CMessageCollectionControllerComp::CreateRepresentationFromObject(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const sdl::imtchat::ImtChat::CMessagesListGqlRequest& listRequest,
		sdl::imtchat::ImtChat::CMessageItem::V1_0& representationObject,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Message collection is not set");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return false;
	}

	const QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtchat::IMessage* msgPtr = nullptr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		msgPtr = dynamic_cast<const imtchat::IMessage*>(dataPtr.GetPtr());
	}

	if (msgPtr == nullptr){
		errorMessage = QString("Unable to create representation from message '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return false;
	}

	Q_UNUSED(listRequest);

	representationObject.id = objectId;
	representationObject.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	representationObject.senderId = msgPtr->GetSenderId();
	representationObject.createdAt = msgPtr->GetCreatedAt();

	const QString content = msgPtr->GetContent();
	if (content.length() > 100){
		representationObject.contentPreview = content.left(100) + QStringLiteral("...");
	}
	else {
		representationObject.contentPreview = content;
	}

	representationObject.hasEntityRefs = !msgPtr->GetEntityReferences().isEmpty();

	return true;
}


istd::IChangeableUniquePtr CMessageCollectionControllerComp::CreateObjectFromRepresentation(
		const sdl::imtchat::ImtChat::CMessageData::V1_0& dataRepresentation,
		QByteArray& newObjectId,
		QString& errorMessage) const
{
	if (!m_messageFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Message factory is not set");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return nullptr;
	}

	imtchat::IMessageUniquePtr msgPtr = m_messageFactCompPtr.CreateInstance();
	if (!msgPtr.IsValid()){
		errorMessage = QStringLiteral("Failed to create message instance");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return nullptr;
	}

	if (!FillObjectFromRepresentation(dataRepresentation, *msgPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return msgPtr;
}


bool CMessageCollectionControllerComp::CreateRepresentationFromObject(
		const istd::IChangeable& data,
		const sdl::imtchat::ImtChat::CMessageItemGqlRequest& itemRequest,
		sdl::imtchat::ImtChat::CMessageData::V1_0& representationPayload,
		QString& errorMessage) const
{
	const imtchat::IMessage* msgPtr = dynamic_cast<const imtchat::IMessage*>(&data);
	if (msgPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IMessage");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return false;
	}

	Q_UNUSED(itemRequest);

	representationPayload.id = msgPtr->GetId();
	representationPayload.conversationId = msgPtr->GetConversationId();
	representationPayload.senderId = msgPtr->GetSenderId();
	representationPayload.content = msgPtr->GetContent();
	representationPayload.createdAt = msgPtr->GetCreatedAt();
	representationPayload.updatedAt = msgPtr->GetUpdatedAt();

	return true;
}


bool CMessageCollectionControllerComp::UpdateObjectFromRepresentationRequest(
		const imtgql::CGqlRequest& /*rawGqlRequest*/,
		const sdl::imtchat::ImtChat::CMessageUpdateGqlRequest& updateRequest,
		istd::IChangeable& object,
		QString& errorMessage) const
{
	imtchat::IMessage* msgPtr = dynamic_cast<imtchat::IMessage*>(&object);
	if (msgPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IMessage");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return false;
	}

	auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}

	if (!arguments.input.Version_1_0->id.has_value()){
		I_CRITICAL();
		return false;
	}

	if (!arguments.input.Version_1_0->item.has_value()){
		I_CRITICAL();
		return false;
	}

	QByteArray objectId = *arguments.input.Version_1_0->id;
	msgPtr->ResetData();
	msgPtr->SetId(objectId);

	sdl::imtchat::ImtChat::CMessageData::V1_0 itemData = *arguments.input.Version_1_0->item;
	return FillObjectFromRepresentation(itemData, *msgPtr, objectId, errorMessage);
}


bool CMessageCollectionControllerComp::FillObjectFromRepresentation(
		const sdl::imtchat::ImtChat::CMessageData::V1_0& representation,
		istd::IChangeable& object,
		QByteArray& objectId,
		QString& errorMessage) const
{
	imtchat::IMessage* msgPtr = dynamic_cast<imtchat::IMessage*>(&object);
	if (msgPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IMessage");
		SendErrorMessage(0, errorMessage, "CMessageCollectionControllerComp");
		return false;
	}

	if (representation.id){
		objectId = *representation.id;
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	msgPtr->SetId(objectId);

	if (representation.conversationId){
		msgPtr->SetConversationId(*representation.conversationId);
	}

	if (representation.senderId){
		msgPtr->SetSenderId(*representation.senderId);
	}

	if (representation.content){
		msgPtr->SetContent(*representation.content);
	}

	if (representation.createdAt){
		msgPtr->SetCreatedAt(*representation.createdAt);
	}

	if (representation.updatedAt){
		msgPtr->SetUpdatedAt(*representation.updatedAt);
	}

	return true;
}


} // namespace imtchatgql
