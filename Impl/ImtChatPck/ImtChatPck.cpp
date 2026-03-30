// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtChatPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtChatPck
{


I_EXPORT_PACKAGE(
			"ImtChatPck",
			"Chat domain model component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ChatMessage,
			"Chat message component",
			"Chat Message");

I_EXPORT_COMPONENT(
			Conversation,
			"Conversation component",
			"Conversation");


} // namespace ImtChatPck
