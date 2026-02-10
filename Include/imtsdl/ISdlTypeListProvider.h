// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


class ISdlTypeListProvider: virtual public istd::IPolymorphic
{

public:
	virtual QStringList GetTypeNames() const = 0;
	/**
	   \returns available types
	   \param onlyLocal - if true - external types will be excluded
	 */
	virtual SdlTypeList GetSdlTypes(bool onlyLocal) const = 0;
	virtual SdlFieldList GetFields(const QString typeName) const = 0;

};


} // namespace imtsdl


