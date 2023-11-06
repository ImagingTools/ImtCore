#include <imtgql/CGqlRepresentationDataControllerComp.h>


// ACF includer
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <imod/TModelWrap.h>


namespace imtgql
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

	istd::TDelPtr<iprm::IParamsSet> representationParamsPtr(CreateContextParams(gqlRequest));

	bool result = m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_dataModelCompPtr, *representationPtr, representationParamsPtr.GetPtr());
	if (result){
		return rootModelPtr.PopPtr();
	}

	errorMessage = QString("Unable to get representation from the data model. Command-ID: %1").arg(qPrintable(gqlRequest.GetCommandId()));

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


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	else if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		const QList<CGqlObject> params = gqlRequest.GetParams();
		if (!params.isEmpty()){
			QByteArray itemData = params.at(0).GetFieldArgumentValue("Item").toByteArray();
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

	errorMessage = QString("Unable to create internal response with command %1").arg(qPrintable(commandId));

	SendErrorMessage(0, errorMessage);

	Q_ASSERT(false);

	return nullptr;
}


iprm::IParamsSet* CGqlRepresentationDataControllerComp::CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray productId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<iprm::CParamsSet> paramsPtr = new imod::TModelWrap<iprm::CParamsSet>();
	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();
		Q_ASSERT(userInfoPtr != nullptr);
		if (userInfoPtr != nullptr){
			istd::TDelPtr<imtauth::IUserInfo> userInfoParamPtr;

			userInfoParamPtr.SetCastedOrRemove(userInfoPtr->CloneMe());
			Q_ASSERT(userInfoParamPtr.IsValid());

			paramsPtr->SetEditableParameter("UserInfo", userInfoParamPtr.PopPtr(), true);
		}

		istd::TDelPtr<iprm::IIdParam> languageIdParam = new iprm::CIdParam();
		languageIdParam->SetId(gqlRequest.GetRequestContext()->GetLanguageId());

		paramsPtr->SetEditableParameter("LanguageParam", languageIdParam.PopPtr(), true);

		istd::TDelPtr<iprm::IIdParam> productIdParam = new iprm::CIdParam();
		productIdParam->SetId(productId);

		paramsPtr->SetEditableParameter("ProductId", productIdParam.PopPtr(), true);
	}

	return paramsPtr.PopPtr();
}


} // namespace imtgql


