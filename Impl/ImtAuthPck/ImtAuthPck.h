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
#include <imtauth/CUserActionMetaInfoCreatorComp.h>
#include <imtauth/CUserActionManagerComp.h>
#include <imtauth/CPersonalAccessToken.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtauth/CPersonalAccessTokenMetaInfoCreatorComp.h>
#include <imtauth/CUserRecentActionComp.h>
#include <imtauth/CTenantInfoComp.h>
#include <imtauth/CTenantMetaInfoCreatorComp.h>
#include <imtauth/CTenantManagerComp.h>
#include <imtauth/CTenantMembershipComp.h>
#include <imtauth/CTenantMembershipManagerComp.h>
#include <imtauth/CTenantEntityBindingComp.h>
#include <imtauth/CTenantEntityBindingManagerComp.h>
#include <imtauth/CTenantInvitationComp.h>
#include <imtauth/CTenantInvitationManagerComp.h>
#include <imtauth/CCrossOrgGrantComp.h>
#include <imtauth/CCrossOrgGrantManagerComp.h>
#include <imtauth/CContractComp.h>
#include <imtauth/CContractManagerComp.h>
#include <imtauth/CTenantRelationshipInfoComp.h>
#include <imtauth/CTenantConnectionRequestComp.h>
#include <imtauth/CTenantConnectionRequestManagerComp.h>
#include <imtauth/CTenantConnectionCodeComp.h>
#include <imtauth/CTenantConnectionComp.h>
#include <imtauth/CTenantRelationshipProposalComp.h>
#include <imtauth/CCrossTenantMessageComp.h>
#include <imtauth/CCrossTenantMessageProjectionComp.h>
#include <imtauth/CCrossTenantMessageBrokerComp.h>
#include <imtauth/COrderRequestComp.h>
#include <imtauth/COrderRequestHandlerComp.h>
#include <imtauth/CTenantDocumentNameProviderComp.h>
#include <imtauth/CTenantRelationshipDocumentNameProviderComp.h>
#include <imtauth/CRoleDocumentNameProviderComp.h>
#include <imtauth/CUserDocumentNameProviderComp.h>
#include <imtauth/CUserGroupDocumentNameProviderComp.h>
#include <imtauth/CTenantDocumentValidatorComp.h>
#include <imtauth/CRelationshipDocumentValidatorComp.h>
#include <imtauth/CCrossOrgGrantDocumentValidatorComp.h>
#include <imtauth/CRoleDocumentValidatorComp.h>
#include <imtauth/CUserDocumentValidatorComp.h>
#include <imtauth/CUserGroupDocumentValidatorComp.h>
#include <imtauth/CDelegatedAccessResolverComp.h>
#include <imtauth/COrganizationsPermissionsProviderComp.h>


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
typedef imtauth::CCollectionPermissionsProviderComp CollectionPermissionsProvider;
typedef imtauth::COrganizationsPermissionsProviderComp OrganizationsPermissionsProvider;
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
typedef imtauth::CUserActionMetaInfoCreatorComp UserActionMetaInfoCreator;
typedef imtauth::CUserActionManagerComp UserActionManager;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtauth::CPersonalAccessToken,
						imtauth::IPersonalAccessToken,
						iser::ISerializable,
						istd::IChangeable>> PersonalAccessToken;
typedef imtauth::CPersonalAccessTokenManagerComp PersonalAccessTokenManager;
typedef imtauth::CPersonalAccessTokenMetaInfoCreatorComp PersonalAccessTokenMetaInfoCreator;
typedef icomp::TModelCompWrap<imtauth::CUserRecentActionComp> UserRecentAction;
typedef icomp::TModelCompWrap<imtauth::CTenantInfoComp> TenantInfo;
typedef imtauth::CTenantMetaInfoCreatorComp TenantMetaInfoCreator;
typedef icomp::TModelCompWrap<imtauth::CTenantManagerComp> TenantManager;
typedef icomp::TModelCompWrap<imtauth::CTenantMembershipComp> TenantMembershipInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantMembershipManagerComp> TenantMembershipManager;
typedef icomp::TModelCompWrap<imtauth::CTenantEntityBindingComp> TenantEntityBindingInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantEntityBindingManagerComp> TenantEntityBindingManager;
typedef icomp::TModelCompWrap<imtauth::CTenantInvitationComp> TenantInvitationInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantInvitationManagerComp> TenantInvitationManager;
typedef icomp::TModelCompWrap<imtauth::CCrossOrgGrantComp> CrossOrgGrantInfo;
typedef icomp::TModelCompWrap<imtauth::CCrossOrgGrantManagerComp> CrossOrgGrantManager;
typedef icomp::TModelCompWrap<imtauth::CContractComp> ContractInfo;
typedef icomp::TModelCompWrap<imtauth::CContractManagerComp> ContractManager;
typedef icomp::TModelCompWrap<imtauth::CTenantConnectionRequestComp> TenantConnectionRequestInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantConnectionRequestManagerComp> TenantConnectionRequestManager;
typedef icomp::TModelCompWrap<imtauth::CTenantConnectionCodeComp> TenantConnectionCodeInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantConnectionComp> TenantConnectionInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantRelationshipProposalComp> TenantRelationshipProposalInfo;
typedef icomp::TModelCompWrap<imtauth::CTenantRelationshipInfoComp> TenantRelationshipInfo;
typedef icomp::TModelCompWrap<imtauth::CCrossTenantMessageComp> CrossTenantMessageInfo;
typedef imtauth::CCrossTenantMessageProjectionComp CrossTenantMessageProjection;
typedef icomp::TModelCompWrap<imtauth::CCrossTenantMessageBrokerComp> CrossTenantMessageBroker;
typedef icomp::TModelCompWrap<imtauth::COrderRequestComp> OrderRequestInfo;
typedef icomp::TModelCompWrap<imtauth::COrderRequestHandlerComp> OrderRequestHandler;
typedef imtauth::CTenantDocumentNameProviderComp TenantDocumentNameProvider;
typedef imtauth::CTenantRelationshipDocumentNameProviderComp TenantRelationshipDocumentNameProvider;
typedef imtauth::CRoleDocumentNameProviderComp RoleDocumentNameProvider;
typedef imtauth::CUserDocumentNameProviderComp UserDocumentNameProvider;
typedef imtauth::CUserGroupDocumentNameProviderComp UserGroupDocumentNameProvider;
typedef imtauth::CTenantDocumentValidatorComp TenantDocumentValidator;
typedef imtauth::CCrossOrgGrantDocumentValidatorComp CrossOrgGrantDocumentValidator;
typedef imtauth::CRelationshipDocumentValidatorComp RelationshipDocumentValidator;
typedef imtauth::CRoleDocumentValidatorComp RoleDocumentValidator;
typedef imtauth::CUserDocumentValidatorComp UserDocumentValidator;
typedef imtauth::CUserGroupDocumentValidatorComp UserGroupDocumentValidator;
typedef imtauth::CDelegatedAccessResolverComp DelegatedAccessResolver;


} // namespace ImtAuthPck

