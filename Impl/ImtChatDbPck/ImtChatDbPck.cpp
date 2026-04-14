// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtChatDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtChatDbPck
{


I_EXPORT_PACKAGE(
			"ImtChatDbPck",
			"Chat database delegate component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AttachmentDbDelegate,
			"Attachment database delegate",
			"Attachment Database Delegate");

I_EXPORT_COMPONENT(
			ConversationDbDelegate,
			"Conversation database delegate",
			"Conversation Database Delegate");

I_EXPORT_COMPONENT(
			MessageDbDelegate,
			"Message database delegate",
			"Message Database Delegate");

I_EXPORT_COMPONENT(
			MessageStatusDbDelegate,
			"Message status database delegate",
			"Message Status Database Delegate");

I_EXPORT_COMPONENT(
			ParticipantDbDelegate,
			"Participant database delegate",
			"Participant Database Delegate");


} // namespace ImtChatDbPck
