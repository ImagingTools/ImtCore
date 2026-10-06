// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtaccount/IContactInfoProvider.h>


namespace imtaccount
{


/**
	A legal entity.
	\ingroup Account
*/
class ICompanyInfo:
	virtual public iser::IObject,
	virtual public IContactInfoProvider
{
public:
	enum MetaInfoTypes
	{
		/**
			Registered name of the company given as QString.
		*/
		MIT_LEGAL_NAME = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	virtual QString GetLegalName() const = 0;
	virtual void SetLegalName(const QString& legalName) = 0;
	virtual const ICompanyInfo* GetParent() const = 0;
	virtual const istd::TPointerVector<const ICompanyInfo>& GetChildren() const = 0;
};


typedef istd::TPointerVector<const ICompanyInfo> CompanyInfoList;


} // namespace imtaccount
