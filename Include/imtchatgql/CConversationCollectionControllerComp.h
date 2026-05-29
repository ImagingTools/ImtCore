// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtchat/IConversation.h>
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ImtChat>


namespace imtchatgql
{


class CConversationCollectionControllerComp:
		public sdl::V1_0::imtchat::CConversationCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtchat::CConversationCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConversationCollectionControllerComp);
		I_ASSIGN(m_conversationFactCompPtr, "ConversationFactory", "Factory for creating conversation instances", true, "ConversationFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtchat::CConversationCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtchat::CConversationsListGqlRequest& listRequest,
				sdl::V1_0::imtchat::CConversationItemData& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtchat::CConversationData& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtchat::CConversationItemGqlRequest& itemRequest,
				sdl::V1_0::imtchat::CConversationData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtchat::CConversationUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::imtchat::CConversationData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtchat::IConversation, m_conversationFactCompPtr);
};


} // namespace imtchatgql
