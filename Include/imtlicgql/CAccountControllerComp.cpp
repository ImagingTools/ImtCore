// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CAccountControllerComp.h>


// ImtCore includes
#include <imtauth/CCompanyInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CAccountControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "CAccountControllerComp");

		return nullptr;
	}

	QByteArray accountId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
		const imtauth::CCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CCompanyInfo*>(dataPtr.GetPtr());
		if (companyInfoPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
			imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

			QString accountName = companyInfoPtr->GetName();
			QString accountDescription = companyInfoPtr->GetDescription();
			QString mail = companyInfoPtr->GetEmail();
			QByteArrayList groups = companyInfoPtr->GetGroups();

			std::sort(groups.begin(), groups.end());

			const imtauth::IAddressProvider* addressProviderPtr = companyInfoPtr->GetAddresses();
			if (addressProviderPtr != nullptr){
				imtbase::ICollectionInfo::Ids addressesIds = addressProviderPtr->GetAddressList().GetElementIds();
				for (const imtbase::ICollectionInfo::Id& addressId : addressesIds){
					const imtauth::IAddress* addressPtr = addressProviderPtr->GetAddress(addressId);
					if (addressPtr != nullptr){
						dataModelPtr->SetData("Country", addressPtr->GetCountry());
						dataModelPtr->SetData("City", addressPtr->GetCity());

						QString postalCodeStr;

						int postalCode = addressPtr->GetPostalCode();
						if (postalCode > 0){
							postalCodeStr = QString::number(postalCode);
						}

						dataModelPtr->SetData("PostalCode", postalCodeStr);
						dataModelPtr->SetData("Street", addressPtr->GetStreet());

						break;
					}
				}
			}

			dataModelPtr->SetData("Id", accountId);
			dataModelPtr->SetData("Name", accountName);
			dataModelPtr->SetData("Description", accountDescription);
			dataModelPtr->SetData("Email", mail);
			dataModelPtr->SetData("Groups", groups.join(';'));

			return rootModelPtr.PopPtr();
		}
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get an account with ID: '%1'.").arg(qPrintable(accountId)));
	SendErrorMessage(0, errorMessage, "CAccountControllerComp");

	return nullptr;
}


istd::IChangeableUniquePtr CAccountControllerComp::CreateObjectFromInputParams(
			const QList<imtgql::CGqlParamObject>& inputParams,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid() || !m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "CAccountControllerComp");

		return nullptr;
	}

	if (!inputParams.isEmpty()){
		objectId = GetObjectIdFromInputParams(inputParams.first());
	}
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParams.at(0).GetParamArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TUniqueInterfacePtr<imtauth::ICompanyInfo> companyInstancePtr = m_accountInfoFactCompPtr.CreateInstance();
		if (!companyInstancePtr.IsValid()){
			errorMessage = QString("Unable to create an instance of the company object.");
			SendErrorMessage(0, errorMessage, "CAccountControllerComp");

			return nullptr;
		}

		imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<imtauth::CIdentifiableCompanyInfo*>(companyInstancePtr.GetPtr());
		if (companyInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an account info!");
			SendErrorMessage(0, errorMessage, "CAccountControllerComp");

			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		companyInfoPtr->SetObjectUuid(objectId);

		QString name;
		QString description;

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (name.isEmpty()){
			errorMessage = QT_TR_NOOP("Account name can't be empty");
			SendErrorMessage(0, errorMessage, "CAccountControllerComp");

			return nullptr;
		}

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				imtauth::IContactBaseInfo* accountInfoPtr = dynamic_cast<imtauth::IContactBaseInfo*>(dataPtr.GetPtr());
				if (accountInfoPtr != nullptr){
					if (collectionId != objectId){
						QString currentName = accountInfoPtr->GetName();
						if (currentName == name){
							errorMessage = QT_TR_NOOP("Account Name already exists");
							return nullptr;
						}
					}
				}
			}
		}

		companyInfoPtr->SetName(name);

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			companyInfoPtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("Email")){
			QString email = itemModel.GetData("Email").toString();
			companyInfoPtr->SetEmail(email);
		}

		if (itemModel.ContainsKey("CompanyName")){
			QString companyName = itemModel.GetData("CompanyName").toString();
		}

		imtauth::CAddress address;
		if (itemModel.ContainsKey("Country")){
			QString country = itemModel.GetData("Country").toString();
			address.SetCountry(country);
		}

		if (itemModel.ContainsKey("City")){
			QString city = itemModel.GetData("City").toString();
			address.SetCity(city);
		}

		if (itemModel.ContainsKey("PostalCode")){
			QString postalCodeStr = itemModel.GetData("PostalCode").toString();

			address.SetPostalCode(postalCodeStr.toInt());
		}

		if (itemModel.ContainsKey("Street")){
			QString street = itemModel.GetData("Street").toString();
			address.SetStreet(street);
		}

		companyInfoPtr->AddAddress(address);

		if (itemModel.ContainsKey("Groups")){
			QByteArray groups = itemModel.GetData("Groups").toByteArray();
			if (!groups.isEmpty()){
				QByteArrayList groupIds = groups.split(';');
				for (const QByteArray& groupId : groupIds){
					companyInfoPtr->AddGroup(groupId);
				}
			}
		}

		return companyInfoPtr;
	}

	errorMessage = QString("Can not create account: %1").arg(QString(objectId));
	SendErrorMessage(0, errorMessage, "CAccountControllerComp");

	return nullptr;
}


} // namespace imtlicgql


