#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/CPP/Roles/Roles.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public sdl::imtauth::Roles::CRoleCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Roles::CRoleCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
		I_ASSIGN(m_roleInfoFactCompPtr, "RoleFactory", "Factory used for creation of the new role instance", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtcore::sdl::Roles::CRoleCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const imtcore::sdl::Roles::CRolesListGqlRequest& rolesListRequest,
				imtcore::sdl::Roles::CRoleItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const imtcore::sdl::Roles::CRoleData& roleDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const imtcore::sdl::Roles::CRoleItemGqlRequest& roleItemRequest,
				imtcore::sdl::Roles::CRoleDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlObject& viewParamsGql,
				iprm::CParamsSet* filterParams) const override;

protected:
	I_ATTR(QByteArray, m_productIdAttrPtr);
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);
};


} // namespace imtauth


