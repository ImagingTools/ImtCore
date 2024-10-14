#include <imtclientgql/CGqlRemoteRepresentationControllerComp.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QString("GraphQL-request is not supported").toUtf8();
		SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.SetCastedOrRemove(gqlRequest.CloneMe());

	if (!gqlRequestPtr.IsValid()){
		errorMessage = QString("GraphQL-request could not be copied").toUtf8();
		SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParam = gqlRequestPtr->GetParamObject("input");
	if (inputParam != nullptr){
		istd::TDelPtr<imtgql::CGqlObject> newInputParamPtr;
		newInputParamPtr.SetCastedOrRemove(inputParam->CloneMe());
		if (!newInputParamPtr.IsValid()){
			errorMessage = QString("Creation of input parameters related to the GraphQL-request failed").toUtf8();
			SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

			return nullptr;
		}

		newInputParamPtr->InsertField("ProductId", QVariant(*m_productIdAttrPtr));
		gqlRequestPtr->AddParam("input", *newInputParamPtr.GetPtr());
	}
	else{
		imtgql::CGqlObject inputGqlObject;
		inputGqlObject.InsertField("ProductId", QVariant(*m_productIdAttrPtr));

		gqlRequestPtr->AddParam("input", inputGqlObject);
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr(dynamic_cast<imtgql::IGqlRequest*>(gqlRequestPtr->CloneMe()));
	if (!requestPtr.isNull()){
		imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			return CreateTreeItemModelFromResponse(*responsePtr);
		}
	}

	errorMessage = QString("Failed to create a network request to a remote server").toUtf8();
	SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

	return nullptr;
}


} // namespace imtclientgql


