// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlUnion.h>


namespace imtsdl
{


class ISdlUnionListProvider: virtual public istd::IPolymorphic
{

public:
	/**
	   \returns available enums
	   \param onlyLocal - if true - external types will be excluded
	 */
	virtual SdlUnionList GetUnions(bool onlyLocal) const = 0;
};


} // namespace imtsdl
