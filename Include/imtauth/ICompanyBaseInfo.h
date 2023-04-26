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
