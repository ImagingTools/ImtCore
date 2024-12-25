#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IRole.h>
#include <imtauth/IPermissionsProvider.h>


namespace imtauthgql
{


class CRoleControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleControllerComp);
		I_ASSIGN(m_roleFactCompPtr, "RoleFactory", "Factory used for creation of the new role", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& objectId, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IRole, m_roleFactCompPtr);
};


} // namespace imtauth


