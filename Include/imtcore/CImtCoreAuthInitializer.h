#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitAuthResourcesGlobal()
{
	Q_INIT_RESOURCE(imtauthdb);
	Q_INIT_RESOURCE(imtauthguilight);
	Q_INIT_RESOURCE(imtauthguidark);

	Q_INIT_RESOURCE(imtauthguiTheme);
	Q_INIT_RESOURCE(imtauthguiqml);

	Q_INIT_RESOURCE(imtauthGroupsSdl);
	Q_INIT_RESOURCE(imtauthRolesSdl);
	Q_INIT_RESOURCE(imtauthUsersSdl);
	Q_INIT_RESOURCE(imtauthProfileSdl);
	Q_INIT_RESOURCE(imtauthSessionsSdl);
	Q_INIT_RESOURCE(imtauthAuthorizationSdl);
	Q_INIT_RESOURCE(imtauthPermissionsSdl);
	Q_INIT_RESOURCE(imtauthPersonalAccessTokensSdl);
	Q_INIT_RESOURCE(imtauthTenantsSdl);
	Q_INIT_RESOURCE(imtauthTenantMembershipsSdl);
	Q_INIT_RESOURCE(imtauthTenantCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtauthRoleCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtauthRelationshipCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtauthGroupCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtauthUserCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtauthCrossOrgGrantCollectionDocumentServiceSdl);
}

namespace imtcore
{


inline void InitializeImtCoreAuth()
{
	ImtCoreInitAuthResourcesGlobal();
}


} // namespace imtcore
