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
			Role,
			"Role",
			"Role");

I_EXPORT_COMPONENT(
			ConnectedProduct,
			"Catalog entry of a connected product",
			"Connected Product");

I_EXPORT_COMPONENT(
			ProductPermissionSnapshot,
			"Permission tree published by a product",
			"Product Permission Snapshot");

I_EXPORT_COMPONENT(
			ConnectedProductDocumentNameProvider,
			"Names a catalog entry after its product",
			"Connected Product Document Name Provider");

I_EXPORT_COMPONENT(
			ConnectedProductDocumentValidator,
			"Validator of the catalog entries of products",
			"Connected Product Document Validator");

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
			OrganizationsPermissionsProvider,
			"Organizations permissions provider",
			"Organizations Permissions Provider");

I_EXPORT_COMPONENT(
			ConnectedClientProductCatalog,
			"Products whose servers may be given a token of this server: the connected products",
			"Connected Client Product Catalog");

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
			PasswordPolicy,
			"Password policy",
			"Password Policy Strength History Lifetime");

I_EXPORT_COMPONENT(
			LdapCredentialController,
			"Ldap credential controller",
			"Ldap Credential Controller");

I_EXPORT_COMPONENT(
			AccountLockoutController,
			"Account lockout controller",
			"Account Lockout Controller");

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
			ExternalIdentity,
			"Link between an external provider account and a user",
			"External Identity OIDC");

I_EXPORT_COMPONENT(
			ExternalIdentityManager,
			"Manager of the links between external provider accounts and users",
			"External Identity Manager OIDC");

I_EXPORT_COMPONENT(
			ExternalLoginController,
			"Sign-in with external identity providers (OAuth 2.0 / OpenID Connect)",
			"External Login Controller OIDC Google Apple Facebook");

I_EXPORT_COMPONENT(
			GoogleOidcProvider,
			"Sign in with Google",
			"Google OIDC Provider");

I_EXPORT_COMPONENT(
			AppleOidcProvider,
			"Sign in with Apple",
			"Apple OIDC Provider");

I_EXPORT_COMPONENT(
			FacebookOidcProvider,
			"Continue with Facebook",
			"Facebook OAuth Provider");

I_EXPORT_COMPONENT(
			GenericOidcProvider,
			"Sign-in with any OpenID Connect or OAuth 2.0 provider",
			"Generic OIDC Provider");

I_EXPORT_COMPONENT(
			TenantInfo,
			"Tenant entity storing organization data (name, description, status, relationships)",
			"Tenant Info");

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
			TenantEntityBindingInfo,
			"Entity representing a tenant-entity binding for visibility filtering",
			"Tenant Entity Binding Info");

I_EXPORT_COMPONENT(
			TenantEntityBindingManager,
			"Manager for tenant-entity bindings with filtered add/remove operations",
			"Tenant Entity Binding Manager");

I_EXPORT_COMPONENT(
			TenantInvitationInfo,
			"Entity representing pending tenant invitation lifecycle state",
			"Tenant Invitation Info");

I_EXPORT_COMPONENT(
			TenantInvitationManager,
			"Manager for tenant invitations and accept/reject/revoke flows",
			"Tenant Invitation Manager");


I_EXPORT_COMPONENT(
			DocumentShareInfo,
			"Entity representing a document shared by one tenant with another",
			"Document Share Info");

I_EXPORT_COMPONENT(
			DocumentShareManager,
			"Manager for document shares between related tenants",
			"Document Share Manager");

I_EXPORT_COMPONENT(
			ContractInfo,
			"Contract of organizations: its terms, parties, links, allocations and the facts of its life",
			"Contract Info");

I_EXPORT_COMPONENT(
			ContractPartyPrivateInfo,
			"Record a party of a contract keeps for itself",
			"Contract Party Private Info");

I_EXPORT_COMPONENT(
			ContractEventInfo,
			"Entry of the journal of a contract",
			"Contract Event Info");

I_EXPORT_COMPONENT(
			ContractManager,
			"Life of the contracts of organizations: drafts, review, signatures, leaving and end",
			"Contract Manager");

I_EXPORT_COMPONENT(
			ContractReminder,
			"Reminds the parties of the deadlines, the ends and the renewals of their contracts",
			"Contract Reminder Deadline");

I_EXPORT_COMPONENT(
			ContractRelationshipProvider,
			"Related organizations as the links of their contracts show them",
			"Contract Relationship Provider");

I_EXPORT_COMPONENT(
			ContractDelegatedAccess,
			"Permissions the users get in other organizations from the contracts of their organizations",
			"Contract Delegated Access");

I_EXPORT_COMPONENT(
			PartyRoleInfo,
			"Role of the parties of an organization",
			"Party Role Info");

I_EXPORT_COMPONENT(
			SystemPartyRole,
			"System role of the parties of contracts declared in an .acc file",
			"System Party Role");

I_EXPORT_COMPONENT(
			PartyRoleCatalog,
			"Catalog of the roles of the parties: system roles and roles of the organizations",
			"Party Role Catalog");

I_EXPORT_COMPONENT(
			ContractTypeInfo,
			"Type of contracts of an organization",
			"Contract Type Info");

I_EXPORT_COMPONENT(
			SystemContractType,
			"System type of contracts declared in an .acc file",
			"System Contract Type");

I_EXPORT_COMPONENT(
			ContractTypeCatalog,
			"Catalog of the types of contracts: system types and types of the organizations",
			"Contract Type Catalog");

I_EXPORT_COMPONENT(
			TenantDirectoryProfile,
			"Profile of an organization in the directory",
			"Tenant Directory Profile");

I_EXPORT_COMPONENT(
			TenantDirectory,
			"Directory of the organizations that want to be found",
			"Tenant Directory");

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
			RoleDocumentNameProvider,
			"Provides default document name from role name",
			"Role Document Name Provider");

I_EXPORT_COMPONENT(
			UserDocumentNameProvider,
			"Provides default document name from user name",
			"User Document Name Provider");

I_EXPORT_COMPONENT(
			UserGroupDocumentNameProvider,
			"Provides default document name from user group name",
			"User Group Document Name Provider");

I_EXPORT_COMPONENT(
			TenantDocumentValidator,
			"Tenant document validator",
			"Tenant Document Validator");


I_EXPORT_COMPONENT(
			RoleDocumentValidator,
			"Role document validator",
			"Role Document Validator");

I_EXPORT_COMPONENT(
			UserDocumentValidator,
			"User document validator",
			"User Document Validator");

I_EXPORT_COMPONENT(
			UserGroupDocumentValidator,
			"User group document validator",
			"User Group Document Validator");


} // namespace ImtAuthPck

