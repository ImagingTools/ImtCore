#include <imtclientgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GraphQL-request is not supported"), "CGqlRemoteRepresentationControllerComp");
		errorMessage = QObject::tr("GraphQL-request is not supported").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.SetCastedOrRemove(gqlRequest.CloneMe());

	if (!gqlRequestPtr.IsValid()){
		SendErrorMessage(0, QString("GraphQL-request could not be copied"), "CGqlRemoteRepresentationControllerComp");
		errorMessage = QObject::tr("GraphQL-request could not be copied").toUtf8();

		return nullptr;
	}

	const imtgql::CGqlObject* inputParam = gqlRequestPtr->GetParam("input");
	if (inputParam != nullptr){
		istd::TDelPtr<imtgql::CGqlObject> newInputParamPtr;
		newInputParamPtr.SetCastedOrRemove(inputParam->CloneMe());
		if (!newInputParamPtr.IsValid()){
			SendErrorMessage(0, QString("Creation of input parameters related to the GraphQL-request failed"), "CGqlRemoteRepresentationControllerComp");
			errorMessage = QObject::tr("Creation of input parameters related to the GraphQL-request failed").toUtf8();

			return nullptr;
		}

		newInputParamPtr->InsertField("ProductId", QVariant(*m_productIdAttrPtr));

		gqlRequestPtr->SetParam(*newInputParamPtr.PopPtr());
	}
	else{
		imtgql::CGqlObject inputGqlObject("input");
		inputGqlObject.InsertField("ProductId", QVariant(*m_productIdAttrPtr));

		gqlRequestPtr->AddParam(inputGqlObject);
	}

	Response responseHandler;
	bool retVal = m_apiClientCompPtr->SendRequest(*gqlRequestPtr.GetPtr(), responseHandler);
	if (retVal){
		return responseHandler.GetResult();
	}

	SendErrorMessage(0, QString("Failed to create a network request to a remote server"), "CGqlRemoteRepresentationControllerComp");
	errorMessage = QObject::tr("Failed to create a network request to a remote server").toUtf8();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* errorsModelPtr = rootModelPtr->AddTreeModel("errors");

	errorsModelPtr->SetData("message", errorMessage);
	errorsModelPtr->SetData("type", "Critical");

	return rootModelPtr.PopPtr();
}


} // namespace imtclientgql


