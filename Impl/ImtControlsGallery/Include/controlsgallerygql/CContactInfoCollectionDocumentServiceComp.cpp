#include <controlsgallerygql/CContactInfoCollectionDocumentServiceComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtauth/IContactInfo.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::controlsgallery::ContactInfos::CContactInfoData CContactInfoCollectionDocumentServiceComp::OnGetContactInfoRepresentation(
			const sdl::controlsgallery::ContactInfoCollectionDocumentService::CGetContactInfoRepresentationGqlRequest& getContactInfoRepresentationRequest,
			const::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::controlsgallery::ContactInfoCollectionDocumentService::GetContactInfoRepresentationRequestArguments arguments = getContactInfoRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::controlsgallery::ContactInfos::CContactInfoData();
	}

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;

		m_documentManagerCompPtr->GetDocumentData("", objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return sdl::controlsgallery::ContactInfos::CContactInfoData();
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		return sdl::controlsgallery::ContactInfos::CContactInfoData();
	}

	sdl::controlsgallery::ContactInfos::CContactInfoData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->firstName = contactInfoPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
	response.Version_1_0->lastName = contactInfoPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
	response.Version_1_0->email = contactInfoPtr->GetEmail();

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CContactInfoCollectionDocumentServiceComp::OnUpdateContactInfoFromRepresentation(
			const sdl::controlsgallery::ContactInfoCollectionDocumentService::CUpdateContactInfoFromRepresentationGqlRequest& updateContactInfoFromRepresentationRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::controlsgallery::ContactInfoCollectionDocumentService::UpdateContactInfoFromRepresentationRequestArguments arguments = updateContactInfoFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::controlsgallery::ContactInfos::CContactInfoData::V1_0 contactInfo;
	if (arguments.input.Version_1_0->contactInfo){
		contactInfo = *arguments.input.Version_1_0->contactInfo;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData("", documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
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

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


sdl::controlsgallery::ContactInfoCollectionDocumentService::CEmailData CContactInfoCollectionDocumentServiceComp::OnGetContactInfoEmailRepresentation(
			const sdl::controlsgallery::ContactInfoCollectionDocumentService::CGetContactInfoEmailRepresentationGqlRequest& getContactInfoEmailRepresentationRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::controlsgallery::ContactInfoCollectionDocumentService::CEmailData response;
	sdl::controlsgallery::ContactInfoCollectionDocumentService::GetContactInfoEmailRepresentationRequestArguments arguments = getContactInfoEmailRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return response;
	}

	QByteArray objectId;
	istd::IChangeableSharedPtr documentPtr;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
		m_documentManagerCompPtr->GetDocumentData("", objectId, documentPtr);
	}

	if (!documentPtr.IsValid()){
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		return response;
	}

	response.Version_1_0.Emplace();
	response.Version_1_0->email = contactInfoPtr->GetEmail();

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CContactInfoCollectionDocumentServiceComp::OnUpdateContactInfoEmailFromRepresentation(
			const sdl::controlsgallery::ContactInfoCollectionDocumentService::CUpdateContactInfoEmailFromRepresentationGqlRequest& updateContactInfoEmailFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::controlsgallery::ContactInfoCollectionDocumentService::UpdateContactInfoEmailFromRepresentationRequestArguments arguments = updateContactInfoEmailFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	sdl::controlsgallery::ContactInfoCollectionDocumentService::CEmailData::V1_0 emailInfo;
	if (arguments.input.Version_1_0->email){
		emailInfo = *arguments.input.Version_1_0->email;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData("", documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::IContactInfo* contactInfoPtr = dynamic_cast<imtauth::IContactInfo*>(documentPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	if (emailInfo.email){
		istd::CChangeGroup changeGroup(contactInfoPtr);

		contactInfoPtr->SetEmail(*emailInfo.email);
	}

	m_documentManagerCompPtr->SetDocumentData("", documentId, *contactInfoPtr);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


} // namespace controlsgallerygql

