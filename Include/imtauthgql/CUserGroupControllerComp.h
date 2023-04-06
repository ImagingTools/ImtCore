#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtcrypt/IHashGenerator.h>


namespace imtauthgql
{


class CUserGroupControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupControllerComp);
		I_ASSIGN(m_userInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group", true, "UserGroupFactory");
		I_ASSIGN(m_rolesCollectionCompPtr, "RolesCollection", "Roles collection", true, "RolesCollection");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IUserGroupInfo, m_userInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_rolesCollectionCompPtr);
};


} // namespace imtauthgql


