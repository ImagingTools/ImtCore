#pragma once

// ImtCore includes
#include <imtauth/IAccountBaseInfo.h>


namespace imtauth
{


class ICompanyInfo: virtual public IAccountBaseInfo
{
public:
	enum MetaInfoTypes
	{
		/**
			Name of company given as QString.
		*/
		MIT_COMPANY_NAME = IAccountBaseInfo::MIT_MAIL + 1,
	};

	// reimplemented (imtauth::IAccountBaseInfo)
	virtual QString GetCompanyName() const = 0;
	virtual void SetCompanyName(const QString& companyName) = 0;
};


} // namespace imtauth
