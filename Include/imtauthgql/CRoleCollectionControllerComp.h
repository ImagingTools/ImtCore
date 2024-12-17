#pragma once


// ImtCore includes
#include <imtauth/CRole.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgql/IGqlRequestProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public sdl::imtauth::Roles::V1_0::CRoleCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Roles::V1_0::CRoleCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_gqlRequestProviderCompPtr, "GqlRequestProvider", "GraphQL request provider", false, "GqlRequestProvider");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
		I_ASSIGN(m_roleInfoFactCompPtr, "RoleFactory", "Factory used for creation of the new role instance", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Roles::V1_0::CRoleCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Roles::V1_0::CRolesListGqlRequest& rolesListRequest,
				sdl::imtauth::Roles::CRoleItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtauth::Roles::CRoleData::V1_0& roleDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Roles::V1_0::CRoleItemGqlRequest& roleItemRequest,
				sdl::imtauth::Roles::CRoleDataPayload::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Roles::V1_0::CRoleUpdateGqlRequest& roleUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	void SetObjectFilter(
				const imtgql::CGqlRequest& gqlRequest,
				const imtbase::CTreeItemModel& objectFilterModel,
				iprm::CParamsSet& filterParams) const override;

protected:
	I_REF(imtgql::IGqlRequestProvider, m_gqlRequestProviderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);
};


} // namespace imtauth


