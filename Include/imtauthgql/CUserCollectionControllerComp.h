#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CUserCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", true, "RoleInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", true, "UserGroupInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
};


} // namespace imtauthgql


