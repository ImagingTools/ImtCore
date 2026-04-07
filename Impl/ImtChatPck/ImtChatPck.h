// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtchat/CChatMessageComp.h>
#include <imtchat/CConversationComp.h>
#include <imtchat/CChatServiceComp.h>


/**
	ImtChatPck package
*/
namespace ImtChatPck
{


typedef icomp::TModelCompWrap<imtchat::CChatMessageComp> ChatMessage;
typedef icomp::TModelCompWrap<imtchat::CConversationComp> Conversation;
typedef imtchat::CChatServiceComp ChatService;


} // namespace ImtChatPck


