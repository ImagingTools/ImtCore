#include <imtlicgql/CAccountControllerComp.h>


// ImtCore includes
#include <imtauth/CAccountInfo.h>


namespace imtlicgql
{

imtbase::CTreeItemModel* CAccountControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	dataModel->SetData("Id", "");
	dataModel->SetData("Name", "");
	dataModel->SetData("Description", "");
	dataModel->SetData("FirstName", "");
	dataModel->SetData("LastName", "");
	dataModel->SetData("NickName", "");
	dataModel->SetData("Email", "");
	dataModel->SetData("BirthDay", "");

	QByteArray accountId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
		const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(dataPtr.GetPtr());
		if (accountInfoPtr != nullptr){
			QString accountName = accountInfoPtr->GetAccountName();
			QString accountDescription = accountInfoPtr->GetAccountDescription();

			QByteArray accountId = accountName.toUtf8();

			QString mail;
			QString lastName;
			QString firstName;
			QString nickName;

			const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();
			if (ownerPtr!= nullptr){
				mail = ownerPtr->GetMail();
				lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
				firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
				nickName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME);
			}

			dataModel->SetData("Id", accountId);
			dataModel->SetData("Name", accountName);
			dataModel->SetData("Description", accountDescription);
			dataModel->SetData("FirstName", firstName);
			dataModel->SetData("LastName", lastName);
			dataModel->SetData("NickName", nickName);
			dataModel->SetData("Email", mail);

			imtbase::CTreeItemModel* addressesModel = new imtbase::CTreeItemModel();
			addressesModel->SetIsArray(true);
			const imtauth::IAddressProvider* addressesPtr = ownerPtr->GetAddresses();
			if (addressesPtr != nullptr){
				imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
				for (const QByteArray& id : ids){
					const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
					if (addressPtr != nullptr){
						int index = addressesModel->InsertNewItem();
						addressesModel->SetData("Country", addressPtr->GetCountry(), index);
						addressesModel->SetData("City", addressPtr->GetCity(), index);
						addressesModel->SetData("PostalCode", addressPtr->GetPostalCode(), index);
						addressesModel->SetData("Street", addressPtr->GetStreet(), index);
					}
				}

				dataModel->SetExternTreeModel("Addresses", addressesModel);
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CAccountControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray &objectId,
			QString &name,
			QString &description,
			QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create account: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<imtauth::CAccountInfo> accountInfoPtr = new imtauth::CAccountInfo();

		if (accountInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			accountInfoPtr->SetAccountName(name);
			objectId = name.toUtf8();

			if (objectId.isEmpty()){
				errorMessage = QT_TR_NOOP("Account name can't be empty!");
				return nullptr;
			}
		}

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			accountInfoPtr->SetAccountDescription(description);
		}

		if (itemModel.ContainsKey("AccountType")){
			QString accountType = itemModel.GetData("AccountType").toString();
			if (accountType == "company"){
				accountInfoPtr->SetAccountType(imtauth::IAccountInfo::AT_COMPANY);
			}
			else if (accountType == "private"){
				accountInfoPtr->SetAccountType(imtauth::IAccountInfo::AT_PERSON);
			}
		}

		imtauth::CContactInfo contactInfo;

		if (itemModel.ContainsKey("Email")){
			QString email = itemModel.GetData("Email").toString();
			contactInfo.SetEmail(email);
		}

		if (itemModel.ContainsKey("LastName")){
			QString lastName = itemModel.GetData("LastName").toString();
			contactInfo.SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, lastName);
		}

		if (itemModel.ContainsKey("FirstName")){
			QString firstName = itemModel.GetData("FirstName").toString();
			contactInfo.SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, firstName);
		}

		if (itemModel.ContainsKey("BirthDay")){
			QString birthDay = itemModel.GetData("BirthDay").toString();
			contactInfo.SetBirthday(QDate::fromString(birthDay));
		}

		if (itemModel.ContainsKey("Gender")){
			QString gender = itemModel.GetData("Gender").toString();

			if (gender == "male"){
				contactInfo.SetGenderType(imtauth::IContactInfo::GenderType::GT_MALE);
			}
			else if (gender == "female"){
				contactInfo.SetGenderType(imtauth::IContactInfo::GenderType::GT_FEMALE);
			}
			else{
				contactInfo.SetGenderType(imtauth::IContactInfo::GenderType::GT_DIVERSE);
			}
		}

		accountInfoPtr->SetAccountOwner(contactInfo);

		return accountInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create account: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace imtlicgql


