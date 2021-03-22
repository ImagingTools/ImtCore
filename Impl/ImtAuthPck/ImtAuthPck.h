#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtauth/CPersonInfo.h>
#include <imtauth/CAddress.h>
#include <imtauth/CAccountInfoComp.h>
#include <imtauth/CPersonInfoMetaInfoCreatorComp.h>
#include <imtauth/CAddressMetaInfoCreatorComp.h>
#include <imtauth/CAccountInfoMetaInfoCreatorComp.h>


namespace ImtAuthPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CPersonInfo,
						imtauth::IPersonInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> PersonInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<imtauth::CAddress>> Address;
typedef icomp::TModelCompWrap<imtauth::CAccountInfoComp> AccountInfo;
typedef imtauth::CPersonInfoMetaInfoCreatorComp PersonInfoMetaInfoCreator;
typedef imtauth::CAddressMetaInfoCreatorComp AddressMetaInfoCreator;
typedef imtauth::CAccountInfoMetaInfoCreatorComp AccountInfoMetaInfoCreator;


} // namespace ImtAuthPck


