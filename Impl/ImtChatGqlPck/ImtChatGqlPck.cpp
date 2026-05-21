// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtChatGqlPck.h"


// ACF includes
#include <icomp/export.h>

// ImtCore includes
#include <imtchatgql/CConversationCollectionControllerComp.h>
#include <imtchatgql/CConversationCollectionDocumentServiceComp.h>


namespace ImtChatGqlPck
{


I_EXPORT_PACKAGE(
			"ImtChatGqlPck",
			"Chat GraphQL controller component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			ConversationCollectionController,
			"Conversation collection controller",
			"Conversation Collection Controller");

I_EXPORT_COMPONENT(
			ConversationCollectionDocumentService,
			"Conversation collection document manager",
			"Conversation Collection Document Manager");


} // namespace ImtChatGqlPck
