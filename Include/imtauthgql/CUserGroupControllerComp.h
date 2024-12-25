#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauthgql
{


class CUserGroupControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupControllerComp);
		I_ASSIGN(m_userGroupInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group", true, "UserGroupFactory");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection", false, "UserCollection");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql


