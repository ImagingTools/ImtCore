#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IRole.h>
#include <imtauth/IPermissionsProvider.h>


namespace imtauthgql
{


class CRoleControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleControllerComp);
		I_ASSIGN(m_roleFactCompPtr, "RoleFactory", "Factory used for creation of the new role", true, "RoleFactory");
//		I_ASSIGN(m_productProviderCompPtr, "ProductProvider", "Product provider", true, "ProductProvider");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IRole, m_roleFactCompPtr);
//	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_productProviderCompPtr);
};


} // namespace imtauth


