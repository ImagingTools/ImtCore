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

I_EXPORT_COMPONENT(
			ChatService,
			"Chat service component",
			"Chat Service");

I_EXPORT_COMPONENT(
			Attachment,
			"Attachment data component",
			"Attachment");

I_EXPORT_COMPONENT(
			AttachmentStorage,
			"Attachment storage service component",
			"Attachment Storage");

I_EXPORT_COMPONENT(
			AttachmentBinaryDataProvider,
			"Binary data provider/controller for HTTP attachment upload/download",
			"Attachment Binary Data Provider");


} // namespace ImtChatPck
