// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtchat/IConversation.h>
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ImtChat.h>


namespace imtchatgql
{


class CConversationCollectionControllerComp:
		public sdl::imtchat::ImtChat::CConversationCollectionControllerCompBase
{
public:
	typedef sdl::imtchat::ImtChat::CConversationCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConversationCollectionControllerComp);
		I_ASSIGN(m_conversationFactCompPtr, "ConversationFactory", "Factory for creating conversation instances", true, "ConversationFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtchat::ImtChat::CConversationCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtchat::ImtChat::CConversationsListGqlRequest& listRequest,
				sdl::imtchat::ImtChat::CConversationItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CConversationData::V1_0& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtchat::ImtChat::CConversationItemGqlRequest& itemRequest,
				sdl::imtchat::ImtChat::CConversationData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtchat::ImtChat::CConversationUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CConversationData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtchat::IConversation, m_conversationFactCompPtr);
};


} // namespace imtchatgql
