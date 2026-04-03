// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtchatdb/CConversationDbDelegateComp.h>
#include <imtchatdb/CMessageDbDelegateComp.h>
#include <imtchatdb/CMessageStatusDbDelegateComp.h>
#include <imtchatdb/CParticipantDbDelegateComp.h>


/**
	ImtChatDbPck package
*/
namespace ImtChatDbPck
{


typedef imtchatdb::CConversationDbDelegateComp ConversationDbDelegate;
typedef imtchatdb::CMessageDbDelegateComp MessageDbDelegate;
typedef imtchatdb::CMessageStatusDbDelegateComp MessageStatusDbDelegate;
typedef imtchatdb::CParticipantDbDelegateComp ParticipantDbDelegate;


} // namespace ImtChatDbPck
