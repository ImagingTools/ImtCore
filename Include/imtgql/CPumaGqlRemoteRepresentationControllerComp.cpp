#include <imtgql/CPumaGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CPumaGqlRemoteRepresentationControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QObject::tr("GraphQL-request is not supported").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.SetCastedOrRemove(gqlRequest.CloneMe());

	if (!gqlRequestPtr.IsValid()){
		errorMessage = QObject::tr("GraphQL-request could not be copied").toUtf8();

		return nullptr;
	}

	const imtgql::CGqlObject* inputParam = gqlRequestPtr->GetParam("input");
	if (inputParam != nullptr){
		istd::TDelPtr<imtgql::CGqlObject> newInputParamPtr;
		newInputParamPtr.SetCastedOrRemove(inputParam->CloneMe());
		if (!newInputParamPtr.IsValid()){
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

	errorMessage = QObject::tr("Failed to create network request").toUtf8();

	return nullptr;
}


} // namespace imtgql


