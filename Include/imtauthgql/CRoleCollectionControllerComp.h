// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtserverapp/CIdParamRepresentationController.h>
#include <imtauth/CRole.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgql/IGqlRequestProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


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
	virtual bool FillObjectFromRepresentation(
				const sdl::imtauth::Roles::CRoleData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::imtauth::Roles::CRoleCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Roles::CRolesListGqlRequest& rolesListRequest,
				sdl::imtauth::Roles::CRoleItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtauth::Roles::CRoleData::V1_0& roleDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Roles::CRoleItemGqlRequest& roleItemRequest,
				sdl::imtauth::Roles::CRoleData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Roles::CRoleUpdateGqlRequest& roleUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, imtbase::CComplexCollectionFilter& complexFilter) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtgql::IGqlRequestProvider, m_gqlRequestProviderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);

private:
	iprm::CIdParam m_idParam;
	istd::TDelPtr<imtserverapp::CIdParamRepresentationController> m_idParamRepresentationControllerPtr;
};


} // namespace imtauth


