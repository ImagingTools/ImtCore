// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtchat/IChatService.h>
#include <imtchat/IConversation.h>
#include <imtbase/IObjectCollection.h>


namespace imtchat
{


class CChatServiceComp:
		public icomp::CComponentBase,
		virtual public IChatService
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CChatServiceComp)
		I_REGISTER_INTERFACE(IChatService);
		I_ASSIGN(m_conversationCollectionCompPtr, "ConversationCollection", "Collection of conversations", true, "ConversationCollection");
		I_ASSIGN(m_conversationFactCompPtr, "ConversationFactory", "Factory for creating conversation objects", true, "ConversationFactory");
	I_END_COMPONENT

	// reimplemented (imtchat::IChatService)
	virtual QByteArray CreateConversation(
				const QString& name,
				int conversationType,
				const QByteArrayList& participantIds) override;
	virtual QByteArrayList GetConversations(int offset = 0, int limit = -1) const override;

private:
	I_REF(imtbase::IObjectCollection, m_conversationCollectionCompPtr);
	I_FACT(IConversation, m_conversationFactCompPtr);
};


} // namespace imtchat
