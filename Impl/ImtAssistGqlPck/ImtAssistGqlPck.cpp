// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAssistGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAssistGqlPck
{


I_EXPORT_PACKAGE(
			"ImtAssistGqlPck",
			"GraphQL API of the AI assistant",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AssistantController,
			"GraphQL controller of the assistant: conversation, confirmation, cancellation",
			"Assistant AI GraphQL Controller");


} // namespace ImtAssistGqlPck


