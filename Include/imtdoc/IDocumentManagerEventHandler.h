// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class IDocumentManagerEventHandler : virtual public istd::IPolymorphic
{
public:
	virtual bool ProcessEvent(CEventBase* eventPtr) = 0;
};


} // namespace imtdoc


