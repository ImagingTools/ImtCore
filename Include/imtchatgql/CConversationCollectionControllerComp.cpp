// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatgql/CConversationCollectionControllerComp.h>

// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtchat/IConversation.h>


namespace imtchatgql
{


// protected methods

bool CConversationCollectionControllerComp::CreateRepresentationFromObject(
		const imtbase::IObjectCollectionIterator& objectCollectionIterator,
		const sdl::imtchat::ImtChat::CConversationsListGqlRequest& listRequest,
		sdl::imtchat::ImtChat::CConversationItemData::V1_0& representationObject,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Conversation collection is not set");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return false;
	}

	const QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtchat::IConversation* convPtr = nullptr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		convPtr = dynamic_cast<const imtchat::IConversation*>(dataPtr.GetPtr());
	}

	if (convPtr == nullptr){
		errorMessage = QString("Unable to create representation from conversation '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return false;
	}

	Q_UNUSED(listRequest);

	representationObject.id = objectId;
	representationObject.typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	representationObject.name = convPtr->GetName();
	representationObject.updatedAt = convPtr->GetUpdatedAt();

	return true;
}


istd::IChangeableUniquePtr CConversationCollectionControllerComp::CreateObjectFromRepresentation(
		const sdl::imtchat::ImtChat::CConversationData::V1_0& dataRepresentation,
		QByteArray& newObjectId,
		QString& errorMessage) const
{
	if (!m_conversationFactCompPtr.IsValid()){
		errorMessage = QStringLiteral("Conversation factory is not set");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return nullptr;
	}

	imtchat::IConversationUniquePtr convPtr = m_conversationFactCompPtr.CreateInstance();
	if (!convPtr.IsValid()){
		errorMessage = QStringLiteral("Failed to create conversation instance");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return nullptr;
	}

	if (!FillObjectFromRepresentation(dataRepresentation, *convPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return convPtr;
}


bool CConversationCollectionControllerComp::CreateRepresentationFromObject(
		const istd::IChangeable& data,
		const sdl::imtchat::ImtChat::CConversationItemGqlRequest& itemRequest,
		sdl::imtchat::ImtChat::CConversationData::V1_0& representationPayload,
		QString& errorMessage) const
{
	const imtchat::IConversation* convPtr = dynamic_cast<const imtchat::IConversation*>(&data);
	if (convPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IConversation");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return false;
	}

	Q_UNUSED(itemRequest);

	representationPayload.id = convPtr->GetId();
	representationPayload.name = convPtr->GetName();
	representationPayload.createdAt = convPtr->GetCreatedAt();
	representationPayload.updatedAt = convPtr->GetUpdatedAt();
	representationPayload.metadata = convPtr->GetMetadata();

	return true;
}


bool CConversationCollectionControllerComp::UpdateObjectFromRepresentationRequest(
		const imtgql::CGqlRequest& /*rawGqlRequest*/,
		const sdl::imtchat::ImtChat::CConversationUpdateGqlRequest& updateRequest,
		istd::IChangeable& object,
		QString& errorMessage) const
{
	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(&object);
	if (convPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IConversation");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
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
	convPtr->ResetData();
	convPtr->SetId(objectId);

	sdl::imtchat::ImtChat::CConversationData::V1_0 itemData = *arguments.input.Version_1_0->item;
	return FillObjectFromRepresentation(itemData, *convPtr, objectId, errorMessage);
}


// private methods

bool CConversationCollectionControllerComp::FillObjectFromRepresentation(
		const sdl::imtchat::ImtChat::CConversationData::V1_0& representation,
		istd::IChangeable& object,
		QByteArray& objectId,
		QString& errorMessage) const
{
	imtchat::IConversation* convPtr = dynamic_cast<imtchat::IConversation*>(&object);
	if (convPtr == nullptr){
		errorMessage = QStringLiteral("Object is not a valid IConversation");
		SendErrorMessage(0, errorMessage, "CConversationCollectionControllerComp");
		return false;
	}

	if (representation.id){
		objectId = *representation.id;
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	convPtr->SetId(objectId);

	if (representation.name){
		convPtr->SetName(*representation.name);
	}

	if (representation.metadata){
		convPtr->SetMetadata(*representation.metadata);
	}

	if (representation.createdAt){
		convPtr->SetCreatedAt(*representation.createdAt);
	}

	if (representation.updatedAt){
		convPtr->SetUpdatedAt(*representation.updatedAt);
	}

	return true;
}


} // namespace imtchatgql
