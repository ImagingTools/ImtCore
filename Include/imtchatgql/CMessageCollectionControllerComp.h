// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtchat/IChatMessage.h>
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ImtChat.h>


namespace imtchatgql
{


class CMessageCollectionControllerComp:
		public sdl::imtchat::ImtChat::CMessageCollectionControllerCompBase
{
public:
	typedef sdl::imtchat::ImtChat::CMessageCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CMessageCollectionControllerComp);
		I_ASSIGN(m_messageFactCompPtr, "MessageFactory", "Factory for creating message instances", true, "MessageFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtchat::ImtChat::CMessageCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtchat::ImtChat::CMessagesListGqlRequest& listRequest,
				sdl::imtchat::ImtChat::CMessageItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CMessageData::V1_0& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtchat::ImtChat::CMessageItemGqlRequest& itemRequest,
				sdl::imtchat::ImtChat::CMessageData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtchat::ImtChat::CMessageUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CMessageData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtchat::IChatMessage, m_messageFactCompPtr);
};


} // namespace imtchatgql
