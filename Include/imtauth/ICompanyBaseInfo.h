// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtauth/IContactBaseInfo.h>


namespace imtauth
{


class ICompanyBaseInfo: virtual public IContactBaseInfo
{
public:
	virtual const ICompanyBaseInfo* GetParent() const = 0;
	virtual const istd::TPointerVector<const ICompanyBaseInfo>& GetChildren() const = 0;
};


typedef istd::TPointerVector<const ICompanyBaseInfo> CompanyInfoList;


} // namespace imtauth
