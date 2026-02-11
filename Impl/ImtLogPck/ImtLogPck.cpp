// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtLogPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLogPck
{


I_EXPORT_PACKAGE(
			"ImtLogPck",
			"ImagingTools log framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			LoginEventController,
			"Login event controller component",
			"Login Event Controller");

I_EXPORT_COMPONENT(
			EventHistoryController,
			"History for events",
			"Event Log History Controller");

I_EXPORT_COMPONENT(
			Storage,
			"Storage for objects",
			"Event Log Cache Storage Object");

I_EXPORT_COMPONENT(
			GroupMessageIdsProvider,
			"Group message ids provider",
			"Group Message Id Provider");

I_EXPORT_COMPONENT(
			EventFilter,
			"Event filter",
			"Event Filter");

I_EXPORT_COMPONENT(
			CollectionBasedMessageConsumer,
			"Collection based message consumer",
			"Collection Message Consumer");

I_EXPORT_COMPONENT(
			MessageComp,
			"Message component",
			"Message");

} // namespace ImtLogPck


