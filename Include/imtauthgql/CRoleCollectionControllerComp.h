// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtserverapp/CIdParamRepresentationController.h>
#include <imtauth/CRole.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgql/IGqlRequestProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public sdl::V1_0::imtauth::CRoleCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CRoleCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_gqlRequestProviderCompPtr, "GqlRequestProvider", "GraphQL request provider", false, "GqlRequestProvider");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
		I_ASSIGN(m_roleInfoFactCompPtr, "RoleFactory", "Factory used for creation of the new role instance", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::V1_0::imtauth::CRoleData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

	// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::imtauth::CRoleCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CRolesListGqlRequest& rolesListRequest,
				sdl::V1_0::imtauth::CRoleItemData& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtauth::CRoleData& roleDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtauth::CRoleItemGqlRequest& roleItemRequest,
				sdl::V1_0::imtauth::CRoleData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtauth::CRoleUpdateGqlRequest& roleUpdateRequest,
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


