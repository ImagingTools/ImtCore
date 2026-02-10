// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlDocumentType.h>


namespace imtsdl
{


class ISdlDocumentTypeListProvider: virtual public istd::IPolymorphic
{

public:
	virtual SdlDocumentTypeList GetDocumentTypes(bool onlyLocal) const = 0;
};


} // namespace imtsdl


