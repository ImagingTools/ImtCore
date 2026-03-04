// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRepresentationDataControllerComp.h>


// ACF include
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <imod/TModelWrap.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtgql::IGqlRepresentationController)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_representationControllerCompPtr.IsValid() || !m_dataModelCompPtr.IsValid()){
		SendCriticalMessage(0, QString("Missing component references: 'DataModel' or 'RepresentationController'"));

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* representationPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(representationPtr != nullptr);

	iprm::IParamsSetUniquePtr representationParamsPtr = CreateContextParams(gqlRequest);

	bool result = m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_dataModelCompPtr, *representationPtr, representationParamsPtr.GetPtr());
	if (result){
		return rootModelPtr.PopPtr();
	}

	errorMessage = QString("Unable to get representation from the data model. Command-ID: '%1'").arg(qPrintable(gqlRequest.GetCommandId()));

	SendErrorMessage(0, errorMessage);

	return nullptr;
}


bool CGqlRepresentationDataControllerComp::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& request,
			imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_representationControllerCompPtr.IsValid() || !m_dataModelCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error"));

		return false;
	}

	bool retVal = m_representationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *m_dataModelCompPtr);
	if (!retVal){
		SendErrorMessage(0, QString("Unable to get data model from representation. Command: %1.").arg(qPrintable(request.GetCommandId())));
	}

	return retVal;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		QList<imtgql::CGqlParamObject> params;
		params.append(gqlRequest.GetParams());
		if (!params.isEmpty()){
			QByteArray itemData = params.at(0).GetParamArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
				if (representationPtr->CreateFromJson(itemData)){
					bool result = UpdateModelFromRepresentation(gqlRequest, representationPtr.GetPtr());
					if (result){
						return representationPtr.PopPtr();
					}
				}
			}
		}
	}

	errorMessage = QString("Unable to create internal response for command '%1'").arg(qPrintable(commandId));

	SendErrorMessage(0, errorMessage);

	Q_ASSERT(false);

	return nullptr;
}


iprm::IParamsSetUniquePtr CGqlRepresentationDataControllerComp::CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray productId;
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetParamArgumentValue("productId").toByteArray();
	}

	if (productId.isEmpty()){
		productId = gqlRequest.GetHeader("productid");
	}

	std::unique_ptr<iprm::CParamsSet> paramsPtr(new imod::TModelWrap<iprm::CParamsSet>());
	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();
		if (userInfoPtr != nullptr){
			istd::TUniqueInterfacePtr<iser::ISerializable> userInfoParamPtr;

			userInfoParamPtr.MoveCastedPtr(userInfoPtr->CloneMe());
			Q_ASSERT(userInfoParamPtr.IsValid());

			paramsPtr->SetEditableParameter("UserInfo", userInfoParamPtr);
		}

		iprm::CIdParam* languageIdParamPtr = new iprm::CIdParam();
		languageIdParamPtr->SetId(gqlRequest.GetRequestContext()->GetLanguageId());

		paramsPtr->SetEditableParameter("LanguageParam", languageIdParamPtr, true);

		iprm::CIdParam* productIdParamPtr = new iprm::CIdParam();
		productIdParamPtr->SetId(productId);

		paramsPtr->SetEditableParameter("ProductId", productIdParamPtr, true);
	}

	return paramsPtr.release();
}


} // namespace imtservergql


