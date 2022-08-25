#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtauth/CContactInfo.h>
#include <imtauth/CAddress.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CContactInfoMetaInfoCreatorComp.h>
#include <imtauth/CAddressMetaInfoCreatorComp.h>
#include <imtauth/CAccountInfoMetaInfoCreatorComp.h>
#include <imtauth/CRoleComp.h>
#include <imtauth/CRoleMetaInfoCreatorComp.h>


namespace ImtAuthPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CContactInfo,
						imtauth::IContactInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> ContactInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<imtauth::CAddress>> Address;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtauth::CAccountInfo,
						imtauth::IAccountInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> AccountInfo;
typedef imtauth::CContactInfoMetaInfoCreatorComp ContactInfoMetaInfoCreator;
typedef imtauth::CAddressMetaInfoCreatorComp AddressMetaInfoCreator;
typedef imtauth::CAccountInfoMetaInfoCreatorComp AccountInfoMetaInfoCreator;
typedef icomp::TModelCompWrap<imtauth::CRoleComp> Role;
typedef imtauth::CRoleMetaInfoCreatorComp RoleMetaInfoCreator;


} // namespace ImtAuthPck


