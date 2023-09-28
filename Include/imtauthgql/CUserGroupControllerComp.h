#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauthgql
{


class CUserGroupControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupControllerComp);
		I_ASSIGN(m_userGroupInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group", true, "UserGroupFactory");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection", true, "UserCollection");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql


