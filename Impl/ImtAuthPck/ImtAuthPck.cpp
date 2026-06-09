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

I_EXPORT_COMPONENT(
			TenantInfo,
			"Tenant entity storing organization data (name, description, status, relationships)",
			"Tenant Info");

I_EXPORT_COMPONENT(
			TenantMetaInfoCreator,
			"Creates meta-information descriptors for tenant entity fields",
			"Tenant Meta Info Creator");

I_EXPORT_COMPONENT(
			TenantManager,
			"CRUD manager for tenant entities (create, read, update, delete, activate/deactivate)",
			"Tenant Manager");


I_EXPORT_COMPONENT(
			TenantMembershipInfo,
			"Entity representing user-to-tenant membership with role (Owner/Admin/Member/Guest)",
			"Tenant Membership Info");

I_EXPORT_COMPONENT(
			TenantMembershipManager,
			"CRUD manager for tenant memberships with role hierarchy and duplicate detection",
			"Tenant Membership Manager");

I_EXPORT_COMPONENT(
			TenantInvitationInfo,
			"Entity representing pending tenant invitation lifecycle state",
			"Tenant Invitation Info");

I_EXPORT_COMPONENT(
			TenantInvitationManager,
			"Manager for tenant invitations and accept/reject/revoke flows",
			"Tenant Invitation Manager");

I_EXPORT_COMPONENT(
			CrossOrgGrantInfo,
			"Entity representing a cross-org grant delegating access between tenants",
			"Cross Org Grant Info");

I_EXPORT_COMPONENT(
			CrossOrgGrantManager,
			"Manager for cross-org grants (create/revoke/query access delegation)",
			"Cross Org Grant Manager");

I_EXPORT_COMPONENT(
			ContractInfo,
			"Entity representing a cooperation contract between two tenants",
			"Contract Info");

I_EXPORT_COMPONENT(
			ContractManager,
			"Manager for cooperation contracts (create/update-status/terminate/query)",
			"Contract Manager");

I_EXPORT_COMPONENT(
			TenantConnectionRequestInfo,
			"Entity representing a tenant connection request used for discovery/bootstrap",
			"Tenant Connection Request Info");

I_EXPORT_COMPONENT(
			TenantConnectionRequestManager,
			"Manager for tenant connection requests and connect codes (create/accept/reject/revoke)",
			"Tenant Connection Request Manager");

I_EXPORT_COMPONENT(
			TenantConnectionCodeInfo,
			"Entity representing a tenant connection code for organization discovery",
			"Tenant Connection Code Info");

I_EXPORT_COMPONENT(
			TenantConnectionInfo,
			"Entity representing an active connection between two tenants",
			"Tenant Connection Info");

I_EXPORT_COMPONENT(
			TenantRelationshipProposalInfo,
			"Entity representing a bilateral relationship proposal between connected tenants",
			"Tenant Relationship Proposal Info");

I_EXPORT_COMPONENT(
			TenantRelationshipInfo,
			"Entity representing a directional relationship between two connected tenants",
			"Tenant Relationship Info");

I_EXPORT_COMPONENT(
			CrossTenantMessageInfo,
			"Entity representing a cross-tenant message exchanged through the messaging pipeline",
			"Cross Tenant Message Info");

I_EXPORT_COMPONENT(
			CrossTenantMessageProjection,
			"Projection applying field whitelist/blacklist to cross-tenant payloads",
			"Cross Tenant Message Projection");

I_EXPORT_COMPONENT(
			CrossTenantMessageBroker,
			"Broker validating, projecting, storing and dispatching cross-tenant messages",
			"Cross Tenant Message Broker");

I_EXPORT_COMPONENT(
			OrderRequestInfo,
			"Entity representing an order request materialized inside the target tenant (Order -> OrderRequest)",
			"Order Request Info");

I_EXPORT_COMPONENT(
			OrderRequestHandler,
			"Domain handler materializing order requests and reporting status feedback to the source tenant",
			"Order Request Handler");

I_EXPORT_COMPONENT(
			TenantDocumentNameProvider,
			"Provides default document name from tenant name",
			"Tenant Document Name Provider");

I_EXPORT_COMPONENT(
			TenantRelationshipDocumentNameProvider,
			"Provides default document name from target tenant name",
			"Tenant Relationship Document Name Provider");

I_EXPORT_COMPONENT(
			CrossOrgGrantDocumentValidator,
			"Cross org grant document validator",
			"Cross Org Grant Document Validator");

I_EXPORT_COMPONENT(
			RelationshipDocumentValidator,
			"Relationship document validator",
			"Relationship Document Validator");

I_EXPORT_COMPONENT(
			DelegatedAccessResolver,
			"Resolver composing membership and cross-org grants for delegated access checks",
			"Delegated Access Resolver");


} // namespace ImtAuthPck

