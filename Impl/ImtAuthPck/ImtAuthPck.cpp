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
			ContactInfo,
			"Contact information",
			"Contact Information Authority");

I_EXPORT_COMPONENT(
			Address,
			"Address information",
			"Address Information Authority");

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
			UserCollection,
			"List of all users",
			"List of all users");

I_EXPORT_COMPONENT(
			UserInfoMetaInfoCreator,
			"User metainfo creator",
			"User Metainfo Creator");

I_EXPORT_COMPONENT(
			RoleCollectionAdapter,
			"Adapting the collection to the provider",
			"Role Collection Adapter");

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


} // namespace ImtAuthPck


