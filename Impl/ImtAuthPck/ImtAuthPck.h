#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtauth/CContactInfo.h>
#include <imtauth/CAddress.h>
#include <imtauth/CAccountInfo.h>
#include <imtauth/CCompanyInfo.h>
#include <imtauth/CContactInfoMetaInfoCreatorComp.h>
#include <imtauth/CAddressMetaInfoCreatorComp.h>
#include <imtauth/CAccountInfoMetaInfoCreatorComp.h>
#include <imtauth/CCompanyInfoMetaInfoCreatorComp.h>
#include <imtauth/CRoleComp.h>
#include <imtauth/CRoleMetaInfoCreatorComp.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserCollection.h>
#include <imtauth/CUserInfoMetaInfoCreatorComp.h>
#include <imtauth/CUserInfoComp.h>
#include <imtauth/CUserGroupInfoComp.h>
#include <imtauth/CRoleCollectionAdapterComp.h>
#include <imtauth/CCheckPermissionCompBase.h>
#include <imtauth/CFeaturePackageRepresentationControllerComp.h>
#include <imtauth/CCollectionPermissionsProviderComp.h>
#include <imtauth/CCheckPermissionOrComp.h>
#include <imtauth/CUserSettingsComp.h>
#include <imtauth/CUserGroupInfoProviderComp.h>
#include <imtauth/CUserCollectionAdapterComp.h>


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
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CIdentifiableCompanyInfo,
						imtauth::IAccountBaseInfo,
						imtauth::ICompanyInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> CompanyInfo;
typedef imtauth::CContactInfoMetaInfoCreatorComp ContactInfoMetaInfoCreator;
typedef imtauth::CAddressMetaInfoCreatorComp AddressMetaInfoCreator;
typedef imtauth::CAccountInfoMetaInfoCreatorComp AccountInfoMetaInfoCreator;
typedef imtauth::CCompanyInfoMetaInfoCreatorComp CompanyInfoMetaInfoCreator;
typedef icomp::TModelCompWrap<imtauth::CRoleComp> Role;
typedef imtauth::CRoleMetaInfoCreatorComp RoleMetaInfoCreator;
typedef icomp::TModelCompWrap<imtauth::CUserInfoComp> UserInfo;
typedef icomp::TModelCompWrap<imtauth::CUserGroupInfoComp> UserGroupInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CUserCollection,
						imtauth::IUserInfoProvider,
						imtauth::IUserInfo,
						iser::ISerializable>> UserCollection;
typedef imtauth::CUserInfoMetaInfoCreatorComp UserInfoMetaInfoCreator;
typedef imtauth::CRoleCollectionAdapterComp RoleCollectionAdapter;
typedef imtauth::CUserCollectionAdapterComp UserCollectionAdapter;
typedef imtauth::CCheckPermissionCompBase CheckPermissions;
typedef imtauth::CFeaturePackageRepresentationControllerComp FeaturePackageRepresentationController;
typedef imtauth::CCollectionPermissionsProviderComp CollectionPermissionsProvider;
typedef imtauth::CCheckPermissionOrComp CheckPermissionOr;
typedef icomp::TModelCompWrap<imtauth::CUserSettingsComp> UserSettings;
typedef imtauth::CUserGroupInfoProviderComp UserGroupInfoProvider;


} // namespace ImtAuthPck


