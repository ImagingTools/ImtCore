// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAuthPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthPck
{


I_EXPORT_PACKAGE(
			"ImtAuthPck",
			"ImagingTools license management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Address,
			"Address information",
			"Address Information Authority");

I_EXPORT_COMPONENT(
			ContactInfo,
			"Contact information",
			"Contact Information");

I_EXPORT_COMPONENT(
			AccountInfo,
			"Account information",
			"Account Information Authority");

I_EXPORT_COMPONENT(
			CompanyInfo,
			"Account company information",
			"Account company Information");

I_EXPORT_COMPONENT(
			ContactInfoMetaInfoCreator,
			"Contact info metainfo creator",
			"Contact Metainfo Creator");

I_EXPORT_COMPONENT(
			AddressMetaInfoCreator,
			"Address metainfo creator",
			"Address Metainfo Creator");

I_EXPORT_COMPONENT(
			AccountInfoMetaInfoCreator,
			"Account info metainfo creator",
			"Account Metainfo Creator");

I_EXPORT_COMPONENT(
			CompanyInfoMetaInfoCreator,
			"Company info metainfo creator",
			"Company Metainfo Creator");

I_EXPORT_COMPONENT(
			Role,
			"Role",
			"Role");

I_EXPORT_COMPONENT(
			RoleMetaInfoCreator,
			"Role metainfo creator",
			"Role Metainfo Creator");

I_EXPORT_COMPONENT(
			UserInfo,
			"User Info",
			"User Info");

I_EXPORT_COMPONENT(
			UserGroupInfo,
			"User group Info",
			"User Group Info");

I_EXPORT_COMPONENT(
			UserInfoMetaInfoCreator,
			"User metainfo creator",
			"User Metainfo Creator");

I_EXPORT_COMPONENT(
			RoleCollectionAdapter,
			"Adapting the collection to the provider",
			"Role Collection Adapter");

I_EXPORT_COMPONENT(
			UserCollectionAdapter,
			"Adapting the collection to the provider",
			"User Collection Adapter");

I_EXPORT_COMPONENT(
			CheckPermissions,
			"Check permissions",
			"Check Permissions");

I_EXPORT_COMPONENT(
			FeaturePackageRepresentationController,
			"Controller for feature package representation",
			"FeaturePackage Package Features Controller Representation");

I_EXPORT_COMPONENT(
			CollectionPermissionsProvider,
			"Collection permissions provider",
			"Collection Permissions Provider");

I_EXPORT_COMPONENT(
			CheckPermissionOr,
			"Check Permission Or",
			"Check Permission Or");

I_EXPORT_COMPONENT(
			UserSettings,
			"User Settings",
			"User Settings");

I_EXPORT_COMPONENT(
			UserGroupInfoProvider,
			"User group provider",
			"UserGroup Group User Provider");

I_EXPORT_COMPONENT(
			SessionInfo,
			"Session info",
			"Session Info");

I_EXPORT_COMPONENT(
			LdapUserCollectionController,
			"Ldap user collection controller",
			"Ldap User Collection Controller");

I_EXPORT_COMPONENT(
			CredentialController,
			"Credential controller",
			"Credential Controller");

I_EXPORT_COMPONENT(
			LdapCredentialController,
			"Ldap credential controller",
			"Ldap Credential Controller");

I_EXPORT_COMPONENT(
			LdapUserCollectionJoiner,
			"Ldap user collection joiner",
			"Ldap User Collection Joiner");

I_EXPORT_COMPONENT(
			UserChangeGenerator,
			"User change generator",
			"User Change Generator");

I_EXPORT_COMPONENT(
			RoleChangeGenerator,
			"Role change generator",
			"Role Change Generator");

I_EXPORT_COMPONENT(
			UserGroupChangeGenerator,
			"User group change generator",
			"User Group Change Generator");

I_EXPORT_COMPONENT(
			UserVerification,
			"User verification",
			"User Verification");

I_EXPORT_COMPONENT(
			UserVerificationController,
			"User verification controller",
			"User Verification Controller");

I_EXPORT_COMPONENT(
			UserVerificationCodeSender,
			"User verification code sender",
			"User Verification Code Sender");

I_EXPORT_COMPONENT(
			JwtSessionController,
			"Json web token session controller",
			"JWT JsonWebToken Session Controller");

I_EXPORT_COMPONENT(
			SessionMetaInfoCreator,
			"Session meta info creator",
			"Session Meta Info Creator");

I_EXPORT_COMPONENT(
			UserMetaInfoCreator,
			"User meta info creator",
			"User Meta Info Creator");

I_EXPORT_COMPONENT(
			GroupMetaInfoCreator,
			"Group meta info creator",
			"Group Meta Info Creator");

I_EXPORT_COMPONENT(
			UserConnectionInfo,
			"User connection info",
			"User Connection Info");

I_EXPORT_COMPONENT(
			UserRecentAction,
			"User recent action",
			"User Recent Action");

I_EXPORT_COMPONENT(
			UserActionMetaInfoCreator,
			"User action meta info creator",
			"User Action Meta Info Creator");

I_EXPORT_COMPONENT(
			UserActionManager,
			"User action manager",
			"User Action manager");

I_EXPORT_COMPONENT(
			PersonalAccessToken,
			"Personal access token",
			"Personal Access Token");

I_EXPORT_COMPONENT(
			PersonalAccessTokenManager,
			"Personal access token manager",
			"Personal Access Token Manager");

I_EXPORT_COMPONENT(
			PersonalAccessTokenMetaInfoCreator,
			"Personal access token meta info creator",
			"Personal Access Token Meta Info Creator");


} // namespace ImtAuthPck


