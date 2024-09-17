#pragma once


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtgql/IGqlRequestProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/CPP/Roles/Roles.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public sdl::imtauth::Roles::CRoleCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Roles::CRoleCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_gqlRequestProviderCompPtr, "GqlRequestProvider", "GraphQL request provider", false, "GqlRequestProvider");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
		I_ASSIGN(m_roleInfoFactCompPtr, "RoleFactory", "Factory used for creation of the new role instance", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Roles::CRoleCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Roles::CRolesListGqlRequest& rolesListRequest,
				sdl::imtauth::Roles::CRoleItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtauth::Roles::CRoleData& roleDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Roles::CRoleItemGqlRequest& roleItemRequest,
				sdl::imtauth::Roles::CRoleDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlObject& viewParamsGql,
				iprm::CParamsSet* filterParams) const override;

protected:
	I_REF(imtgql::IGqlRequestProvider, m_gqlRequestProviderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);
};


} // namespace imtauth


