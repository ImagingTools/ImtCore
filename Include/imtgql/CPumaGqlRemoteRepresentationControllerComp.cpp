#include <imtgql/CPumaGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CPumaGqlRemoteRepresentationControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!IsRequestSupported(gqlRequest)){
		return nullptr;
	}

	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.SetCastedOrRemove(gqlRequest.CloneMe());

	if (!gqlRequestPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlObject* inputParam = gqlRequestPtr->GetParam("input");
	if (inputParam != nullptr){
		istd::TDelPtr<imtgql::CGqlObject> newInputParamPtr;
		newInputParamPtr.SetCastedOrRemove(inputParam->CloneMe());
		if (!newInputParamPtr.IsValid()){
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

	return nullptr;
}


} // namespace imtgql


