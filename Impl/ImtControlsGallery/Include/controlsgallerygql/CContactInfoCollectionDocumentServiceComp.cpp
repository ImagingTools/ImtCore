#include <controlsgallerygql/CContactInfoCollectionDocumentServiceComp.h>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtauth/IContactInfo.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (CContactInfoCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::controlsgallery::CContactInfoData CContactInfoCollectionDocumentServiceComp::OnGetContactInfoRepresentation(
			const sdl::V1_0::controlsgallery::CGetContactInfoRepresentationGqlRequest& getContactInfoRepresentationRequest,
			const::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::controlsgallery::GetContactInfoRepresentationRequestArguments arguments = getContactInfoRepresentationRequest.GetRequestedArguments();
	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.id){
		objectId = *arguments.input.id;

		m_documentManagerCompPtr->GetDocumentData("", objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::V1_0::controlsgallery::CContactInfoData();
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		return sdl::V1_0::controlsgallery::CContactInfoData();
	}

	sdl::V1_0::controlsgallery::CContactInfoData response;

	response.firstName = contactInfoPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
	response.lastName = contactInfoPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
	response.email = contactInfoPtr->GetEmail();

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CContactInfoCollectionDocumentServiceComp::OnUpdateContactInfoFromRepresentation(
			const sdl::V1_0::controlsgallery::CUpdateContactInfoFromRepresentationGqlRequest& updateContactInfoFromRepresentationRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::controlsgallery::UpdateContactInfoFromRepresentationRequestArguments arguments = updateContactInfoFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.documentId){
		documentId = *arguments.input.documentId;
	}

	sdl::V1_0::controlsgallery::CContactInfoData contactInfo;
	if (arguments.input.contactInfo){
		contactInfo = *arguments.input.contactInfo;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData("", documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	istd::CChangeGroup changeGroup(contactInfoPtr);

	if (contactInfo.firstName){
		contactInfoPtr->SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, *contactInfo.firstName);
	}

	if (contactInfo.lastName){
		contactInfoPtr->SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, *contactInfo.lastName);
	}

	if (contactInfo.email){
		contactInfoPtr->SetEmail(*contactInfo.email);
	}

	m_documentManagerCompPtr->SetDocumentData("", documentId, *contactInfoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


sdl::V1_0::controlsgallery::CEmailData CContactInfoCollectionDocumentServiceComp::OnGetContactInfoEmailRepresentation(
			const sdl::V1_0::controlsgallery::CGetContactInfoEmailRepresentationGqlRequest& getContactInfoEmailRepresentationRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::controlsgallery::CEmailData response;
	sdl::V1_0::controlsgallery::GetContactInfoEmailRepresentationRequestArguments arguments = getContactInfoEmailRepresentationRequest.GetRequestedArguments();
	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.id){
		objectId = *arguments.input.id;
		m_documentManagerCompPtr->GetDocumentData("", objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		return response;
	}

	response.email = contactInfoPtr->GetEmail();

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CContactInfoCollectionDocumentServiceComp::OnUpdateContactInfoEmailFromRepresentation(
			const sdl::V1_0::controlsgallery::CUpdateContactInfoEmailFromRepresentationGqlRequest& updateContactInfoEmailFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::controlsgallery::UpdateContactInfoEmailFromRepresentationRequestArguments arguments = updateContactInfoEmailFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.documentId){
		documentId = *arguments.input.documentId;
	}

	sdl::V1_0::controlsgallery::CEmailData emailInfo;
	if (arguments.input.email){
		emailInfo = *arguments.input.email;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData("", documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	if (emailInfo.email){
		istd::CChangeGroup changeGroup(contactInfoPtr);

		contactInfoPtr->SetEmail(*emailInfo.email);
	}

	m_documentManagerCompPtr->SetDocumentData("", documentId, *contactInfoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace controlsgallerygql

