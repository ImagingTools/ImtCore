#include <controlsgallerygql/CContactInfoCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CContactInfo.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)

sdl::V1_0::imtbase::CVisualStatus CContactInfoCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CVisualStatus response;

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return sdl::V1_0::imtbase::CVisualStatus();
	}

	sdl::V1_0::imtbase::GetObjectVisualStatusRequestArguments arguments = getObjectVisualStatusRequest.GetRequestedArguments();

	QByteArray objectId;
	if (arguments.input.objectId.has_value()){
		objectId = *arguments.input.objectId;
	}

	response.objectId = objectId;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtauth::CContactInfo* contactInfoPtr = dynamic_cast<const imtauth::CContactInfo*>(dataPtr.GetPtr());
		if (contactInfoPtr != nullptr){
			QString firstName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_FIRST_NAME);
			QString lastName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_LAST_NAME);
			
			response.text = firstName + QStringLiteral(" ") + lastName;
		}
	}

	return response;
}


// reimplemented (sdl::V1_0::controlsgallery::CContactInfoCollectionControllerCompBase)

bool CContactInfoCollectionControllerComp::CreateRepresentationFromObject(
			const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::controlsgallery::CContactInfoListGqlRequest& contactInfoListRequest,
			sdl::V1_0::controlsgallery::CContactInfoItemData& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return false;
	}
	
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::CContactInfo* contactInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		contactInfoPtr = dynamic_cast<const imtauth::CContactInfo*>(dataPtr.GetPtr());
	}

	if (contactInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CContactInfoCollectionControllerComp");

		return false;
	}

	sdl::V1_0::controlsgallery::ContactInfoListRequestInfo requestInfo = contactInfoListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = objectCollectionIterator.GetObjectTypeId();
		representationObject.typeId = collectionObjectId;
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = (added);
	}

	if (requestInfo.items.isFirstNameRequested){
		representationObject.firstName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_FIRST_NAME);
	}

	if (requestInfo.items.isLastNameRequested){
		representationObject.lastName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_LAST_NAME);
	}

	if (requestInfo.items.isEmailRequested){
		representationObject.email = contactInfoPtr->GetEmail();
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("Timestamp").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = (lastModified);
	}

	return true;
}


bool CContactInfoCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::controlsgallery::CGetContactInfoGqlRequest& getContactInfoRequest,
			sdl::V1_0::controlsgallery::CContactInfoData& representationPayload,
			QString& errorMessage) const
{
	const imtauth::CContactInfo* contactInfoPtr = dynamic_cast<const imtauth::CContactInfo*>(&data);
	if (contactInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CContactInfoCollectionControllerComp");
		return false;
	}

	sdl::V1_0::controlsgallery::GetContactInfoRequestArguments arguments = getContactInfoRequest.GetRequestedArguments();

	QByteArray id;
	if (arguments.input.id){
		id = *arguments.input.id;
	}

	representationPayload.id = id;
	representationPayload.email = contactInfoPtr->GetEmail();
	representationPayload.firstName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_FIRST_NAME);
	representationPayload.lastName = contactInfoPtr->GetNameField(imtauth::CContactInfo::NFT_LAST_NAME);

	imtauth::CContactInfo::GenderType genderType = contactInfoPtr->GetGenderType();
	switch (genderType){
	case imtauth::IContactInfo::GT_DIVERSE:
		representationPayload.genderType = sdl::V1_0::controlsgallery::GenderType::DIVERSE;
		break;
	case imtauth::IContactInfo::GT_FEMALE:
		representationPayload.genderType = sdl::V1_0::controlsgallery::GenderType::FEMALE;
	case imtauth::IContactInfo::GT_MALE:
		break;
		representationPayload.genderType = sdl::V1_0::controlsgallery::GenderType::MALE;
		break;
	}

	QDate birthday = contactInfoPtr->GetBirthday();
	representationPayload.birthday = birthday.toString(Qt::DateFormat::ISODate);

	return true;
}


istd::IChangeableUniquePtr CContactInfoCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::controlsgallery::CContactInfoData& contactInfoDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	int typeIdIndex = GetObjectTypeIdIndex("ContactInfo");
	if (typeIdIndex < 0){
		Q_ASSERT_X(false, "Type-ID: 'ContactInfo' unregistered", "CDeviceCollectionControllerComp");
		return nullptr;
	}
	
	istd::TUniqueInterfacePtr<istd::IChangeable> contactInstanceInfoPtr = m_objectFactCompPtr.CreateInstance(typeIdIndex);
	if (!contactInstanceInfoPtr.IsValid()){
		errorMessage = QString("Unable to create contact info instance. Error: Invalid object");
		return nullptr;
	}

	imtauth::CContactInfo* contactInfoPtr = dynamic_cast<imtauth::CContactInfo*>(contactInstanceInfoPtr.GetPtr());
	if (contactInfoPtr == nullptr){
		errorMessage = QString("Unable to cast object to contact info. Error: Invalid object");
		return nullptr;
	}

	QByteArray objectId;
	if (!FillObjectFromRepresentation(contactInfoDataRepresentation, *contactInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	return contactInstanceInfoPtr.PopInterfacePtr();
}


bool CContactInfoCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& rawGqlRequest,
			const sdl::V1_0::controlsgallery::CUpdateContactInfoGqlRequest& updateContactInfoRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return false;
	}

	sdl::V1_0::controlsgallery::UpdateContactInfoRequestArguments inputArguments = updateContactInfoRequest.GetRequestedArguments();
	if (!inputArguments.input.item){
		I_CRITICAL();
		return false;
	}

	imtauth::CContactInfo* contactInfoPtr = dynamic_cast<imtauth::CContactInfo*>(&object);
	Q_ASSERT(contactInfoPtr != nullptr);

	sdl::V1_0::controlsgallery::CContactInfoData contactInfoData = *inputArguments.input.item;

	QByteArray objectId;
	if (!FillObjectFromRepresentation(contactInfoData, *contactInfoPtr, objectId, errorMessage)){
		return false;
	}

	return true;
}


// private methods

bool CContactInfoCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::V1_0::controlsgallery::CContactInfoData& representation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& /*errorMessage*/) const
{
	imtauth::CContactInfo* contactInfoPtr = dynamic_cast<imtauth::CContactInfo*>(&object);
	Q_ASSERT(contactInfoPtr != nullptr);

	if (representation.id){
		objectId = *representation.id;
	}

	if (representation.email){
		contactInfoPtr->SetEmail(*representation.email);
	}

	if (representation.firstName){
		contactInfoPtr->SetNameField(imtauth::CContactInfo::NFT_FIRST_NAME, *representation.firstName);
	}

	if (representation.lastName){
		contactInfoPtr->SetNameField(imtauth::CContactInfo::NFT_LAST_NAME, *representation.lastName);
	}

	if (representation.birthday){
		QString birthdayStr = *representation.birthday;
		QDate birthday = QDate::fromString(birthdayStr, Qt::DateFormat::ISODate);
		contactInfoPtr->SetBirthday(birthday);
	}

	if (representation.genderType){
		sdl::V1_0::controlsgallery::GenderType genderType = *representation.genderType;
		switch (genderType){
		case sdl::V1_0::controlsgallery::GenderType::DIVERSE:
			contactInfoPtr->SetGenderType(imtauth::CContactInfo::GT_DIVERSE);
			break;
		case sdl::V1_0::controlsgallery::GenderType::MALE:
			contactInfoPtr->SetGenderType(imtauth::CContactInfo::GT_MALE);
			break;
		case sdl::V1_0::controlsgallery::GenderType::FEMALE:
			contactInfoPtr->SetGenderType(imtauth::CContactInfo::GT_FEMALE);
			break;
		}
	}

	return true;
}


} // namespace controlsgallerygql


