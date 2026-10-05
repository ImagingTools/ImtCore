// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtaccount/CAddress.h>
#include <imtaccount/CAccountInfo.h>
#include <imtaccount/CContactInfo.h>
#include <imtaccount/CCompanyInfo.h>
#include <imtaccount/CAccountInfoMetaInfoCreatorComp.h>
#include <imtaccount/CContactInfoMetaInfoCreatorComp.h>


namespace ImtAccountPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<imtaccount::CAddress>> Address;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtaccount::CAccountInfo,
						imtaccount::IAccountInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> AccountInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtaccount::CContactInfo,
						imtaccount::IContactInfo,
						iser::ISerializable,
						istd::IChangeable>> ContactInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtaccount::CIdentifiableCompanyInfo,
						imtaccount::IContactBaseInfo,
						imtaccount::ICompanyInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> CompanyInfo;
typedef imtaccount::CAccountInfoMetaInfoCreatorComp AccountInfoMetaInfoCreator;
typedef imtaccount::CContactInfoMetaInfoCreatorComp ContactInfoMetaInfoCreator;


} // namespace ImtAccountPck
