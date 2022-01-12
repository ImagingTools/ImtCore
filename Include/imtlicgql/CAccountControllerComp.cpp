#include <imtlicgql/CAccountControllerComp.h>

// ImtCore includes
//#include <imtlic/CFeaturePackageCollectionUtility.h>
//#include <imtlic/CFeatureInfo.h>
//#include <imtlic/IFeaturePackage.h>
#include <imtauth/IContactInfo.h>
#include <imtauth/IAccountInfo.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>



namespace imtlicgql
{


imtbase::CTreeItemModel* CAccountControllerComp::GetObject(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemModel = nullptr;
	bool isSetResponce = false;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemModel = new imtbase::CTreeItemModel();

		QByteArray accountId = GetObjectIdFromInputParams(inputParams);

		QByteArrayList featureIds;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
			const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(dataPtr.GetPtr());

//			const imtauth::IContactInfo* contactPtr = dynamic_cast<const imtauth::IContactInfo*>(dataPtr.GetPtr());
			if (accountInfoPtr != nullptr){
				QString accountName = accountInfoPtr->GetAccountName();

				QByteArray accountId = accountName.toUtf8();

				QByteArray accountTypeId;
				imtauth::IAccountInfo::AccountType accountType = accountInfoPtr->GetAccountType();
				if (accountType == imtauth::IAccountInfo::AT_COMPANY){
					accountTypeId = "company";
				}
				else if (accountType == imtauth::IAccountInfo::AT_PERSON){
					accountTypeId = "private";
				}

				QString accountDescription = accountInfoPtr->GetAccountDescription();

				QString mail;
				QString lastName;
				QString firstName;
				QString nickName;

				const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();
				if (ownerPtr != nullptr){
					mail = ownerPtr->GetMail();
					lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
					firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
					nickName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME);

				}

				itemModel->SetData("Id", accountId);
				itemModel->SetData("FirstName", firstName);
				itemModel->SetData("LastName", lastName);
				itemModel->SetData("NickName", nickName);
				itemModel->SetData("Email", mail);
				itemModel->SetData("AccountType", accountTypeId);
				itemModel->SetData("AccountDescription", accountDescription);

				imtbase::CTreeItemModel* addressesModel = new imtbase::CTreeItemModel();
				addressesModel->SetIsArray(true);
				const imtauth::IAddressProvider* addressesPtr = ownerPtr->GetAddresses();
				if (addressesPtr != nullptr){
					imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
					for (const QByteArray& id : ids){
						const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
						if (addressPtr != nullptr){
							int index = addressesModel->InsertNewItem();
							addressesModel->SetData("Country", addressPtr->GetCountry(),index);
							addressesModel->SetData("City", addressPtr->GetCity(), index);
							addressesModel->SetData("PostalCode", addressPtr->GetPostalCode(), index);
							addressesModel->SetData("Street", addressPtr->GetStreet(), index);
							//addressesModel->SetExternTreeModel("address", addressModel, index);
						}
					}
					itemModel->SetExternTreeModel("Addresses", addressesModel);
				}
			}
		}

//		itemsModel->SetIsArray(true);

		dataModel->SetExternTreeModel("item", itemModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


