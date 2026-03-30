// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtchat/ITicket.h>
#include <GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ImtChat.h>


namespace imtchatgql
{


class CTicketCollectionControllerComp:
		public sdl::imtchat::ImtChat::CTicketCollectionControllerCompBase
{
public:
	typedef sdl::imtchat::ImtChat::CTicketCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTicketCollectionControllerComp);
		I_ASSIGN(m_ticketFactCompPtr, "TicketFactory", "Factory for creating ticket instances", true, "TicketFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtchat::ImtChat::CTicketCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtchat::ImtChat::CTicketsListGqlRequest& listRequest,
				sdl::imtchat::ImtChat::CTicketItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CTicketData::V1_0& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtchat::ImtChat::CTicketItemGqlRequest& itemRequest,
				sdl::imtchat::ImtChat::CTicketData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtchat::ImtChat::CTicketUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::imtchat::ImtChat::CTicketData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(imtchat::ITicket, m_ticketFactCompPtr);
};


} // namespace imtchatgql
