// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtauth/CSessionInfo.h>
#include <imtauth/CLdapUserCollectionControllerComp.h>
#include <imtauth/CLdapCredentialControllerComp.h>
#include <imtauth/CCredentialControllerComp.h>
#include <imtauth/CLdapUserCollectionJoinerComp.h>
#include <imtauth/CUserChangeGeneratorComp.h>
#include <imtauth/CRoleChangeGeneratorComp.h>
#include <imtauth/CUserGroupChangeGeneratorComp.h>
#include <imtauth/CUserVerification.h>
#include <imtauth/CUserVerificationControllerComp.h>
#include <imtauth/CUserVerificationCodeSenderComp.h>
#include <imtauth/CJwtSessionControllerComp.h>
#include <imtauth/CSessionMetaInfoCreatorComp.h>
#include <imtauth/CUserMetaInfoCreatorComp.h>
#include <imtauth/CGroupMetaInfoCreatorComp.h>
#include <imtauth/CUserConnectionInfo.h>
#include <imtauth/CUserRecentAction.h>
#include <imtauth/CUserActionMetaInfoCreatorComp.h>
#include <imtauth/CUserActionManagerComp.h>
#include <imtauth/CPersonalAccessToken.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>


namespace ImtAuthPck
{


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
			icomp::TMakeComponentWrap <
						imtauth::CContactInfo,
						imtauth::IContactInfo,
						iser::ISerializable,
						istd::IChangeable>> ContactInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CIdentifiableCompanyInfo,
						imtauth::IContactBaseInfo,
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
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
				imtauth::CIdentifiableUserVerificationInfo,
				imtauth::IUserVerification,
				iser::ISerializable,
				istd::IChangeable>> UserVerification;
typedef icomp::TModelCompWrap<imtauth::CUserGroupInfoComp> UserGroupInfo;
typedef imtauth::CUserInfoMetaInfoCreatorComp UserInfoMetaInfoCreator;
typedef imtauth::CRoleCollectionAdapterComp RoleCollectionAdapter;
typedef imtauth::CUserCollectionAdapterComp UserCollectionAdapter;
typedef imtauth::CCheckPermissionCompBase CheckPermissions;
typedef imtauth::CFeaturePackageRepresentationControllerComp FeaturePackageRepresentationController;
typedef imtauth::CCollectionPermissionsProviderComp CollectionPermissionsProvider;
typedef imtauth::CCheckPermissionOrComp CheckPermissionOr;
typedef icomp::TModelCompWrap<imtauth::CUserSettingsComp> UserSettings;
typedef imtauth::CUserGroupInfoProviderComp UserGroupInfoProvider;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtauth::CSessionInfo,
						imtauth::ISession,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> SessionInfo;
typedef imtauth::CLdapUserCollectionControllerComp LdapUserCollectionController;
typedef imtauth::CCredentialControllerComp CredentialController;
typedef imtauth::CLdapCredentialControllerComp LdapCredentialController;
typedef imtauth::CLdapUserCollectionJoinerComp LdapUserCollectionJoiner;
typedef imtauth::CUserChangeGeneratorComp UserChangeGenerator;
typedef imtauth::CRoleChangeGeneratorComp RoleChangeGenerator;
typedef imtauth::CUserGroupChangeGeneratorComp UserGroupChangeGenerator;
typedef imtauth::CUserVerificationControllerComp UserVerificationController;
typedef imtauth::CUserVerificationCodeSenderComp UserVerificationCodeSender;
typedef imtauth::CJwtSessionControllerComp JwtSessionController;
typedef imtauth::CSessionMetaInfoCreatorComp SessionMetaInfoCreator;
typedef imtauth::CUserMetaInfoCreatorComp UserMetaInfoCreator;
typedef imtauth::CGroupMetaInfoCreatorComp GroupMetaInfoCreator;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CUserConnectionInfo,
						imtauth::IUserConnectionInfo,
						iser::ISerializable,
						istd::IChangeable>> UserConnectionInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtauth::CIdentifiableUserRecentAction,
						imtauth::IUserRecentAction,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> UserRecentAction;
typedef imtauth::CUserActionMetaInfoCreatorComp UserActionMetaInfoCreator;
typedef imtauth::CUserActionManagerComp UserActionManager;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtauth::CPersonalAccessToken,
						imtauth::IPersonalAccessToken,
						iser::ISerializable,
						istd::IChangeable>> PersonalAccessToken;
typedef imtauth::CPersonalAccessTokenManagerComp PersonalAccessTokenManager;


} // namespace ImtAuthPck


