#include <imtauthgui/CContactInfoRepresentationDataController.h>

// ACF Includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>
#include <imtauth/IContactInfo.h>
#include <imtbase/CTreeItemModel.h>



namespace imtauthgui
{


CContactInfoRepresentationDataController::CContactInfoRepresentationDataController()
	: imtbase::IItemBasedRepresentationDataController()
{
	m_isUpdateFromRepresentation = false;
}


void CContactInfoRepresentationDataController::UpdateRepresentationFromBaseModel(
		istd::IChangeable *baseModel,
		istd::IChangeable *representationModel,
		const istd::IChangeable::ChangeSet &changeSet)
{
	if (m_isUpdateFromRepresentation == true){
		return;
	}

	imtbase::CTreeItemModel* treeItemModel = dynamic_cast<imtbase::CTreeItemModel*>(representationModel);
	imtauth::IContactInfo* contactPtr = dynamic_cast<imtauth::IContactInfo*>(baseModel);

	Q_ASSERT(treeItemModel != nullptr);
	Q_ASSERT(contactPtr != nullptr);

	istd::IChangeable::ChangeInfoMap infoMap = changeSet.GetChangeInfoMap();
	if (infoMap.isEmpty()){
		this->FullUpdateRepresentationModel(contactPtr, treeItemModel);
	}
	else{
		if (infoMap.contains("key0") && infoMap.value("key0") == "addresses"){
			treeItemModel = treeItemModel->GetTreeItemModel("addresses",infoMap.value("index0",0).toInt());
		}
		istd::IChangeable::ChangeInfoMap infoUpdate;
		infoUpdate.insert("fromModel",true);
		treeItemModel->SetData(infoMap.value("key").toByteArray(),infoMap.value("value"), infoMap.value("index").toInt(), infoUpdate);
	}
}


void CContactInfoRepresentationDataController::UpdateBaseModelFromRepresentation(
		istd::IChangeable *baseModel,
		istd::IChangeable *representationModel,
		const istd::IChangeable::ChangeSet &changeSet)
{
	istd::IChangeable::ChangeInfoMap infoMap = changeSet.GetChangeInfoMap();
	if (infoMap.contains("fromModel")){
		return;
	}
	imtbase::CTreeItemModel* treeItemModel = dynamic_cast<imtbase::CTreeItemModel*>(representationModel);
	imtauth::IContactInfo* contactPtr = dynamic_cast<imtauth::IContactInfo*>(baseModel);

	Q_ASSERT(treeItemModel != nullptr);
	Q_ASSERT(contactPtr != nullptr);

	if (infoMap.isEmpty()){
		this->FullUpdateBaseModel(contactPtr, treeItemModel);
	}
	else{
		m_isUpdateFromRepresentation = true;

		if (infoMap.value("key") == "firstName"){
			contactPtr->SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, treeItemModel->GetData("firstName").toString());
		}
		if (infoMap.value("key") == "lastName"){
			contactPtr->SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, treeItemModel->GetData("lastName").toString());
		}
		if (infoMap.value("key") == "nickName"){
			contactPtr->SetNameField(imtauth::IContactInfo::NFT_NICKNAME, treeItemModel->GetData("nickName").toString());
		}

		if (infoMap.value("key0") == "addresses"){
			imtbase::CTreeItemModel* addressesModel = treeItemModel->GetTreeItemModel("addresses",infoMap.value("index0",0).toInt());
			imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
						const_cast<imtauth::IAddressProvider*>(contactPtr->GetAddresses()));

			if (addressesPtr != nullptr && addressesModel != nullptr){
				QByteArray id = treeItemModel->GetData("addressId").toByteArray();
//				imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
//				int index = infoMap.value("index").toInt();
//				if (infoMap.value("key") == "country"){
//					addressPtr->SetCountry(addressesModel->GetData("country",index).toString());
//				}
//				if (infoMap.value("key") == "city"){
//					addressPtr->SetCity(addressesModel->GetData("city",index).toString());
//				}
//				if (infoMap.value("key") == "postalCode"){
//					addressPtr->SetPostalCode(addressesModel->GetData("postalCode",index).toInt());
//				}
//				if (infoMap.value("key") == "street"){
//					addressPtr->SetStreet(addressesModel->GetData("street",index).toString());
//				}
			}
		}
		this->m_isUpdateFromRepresentation = false;

	}
}


void CContactInfoRepresentationDataController::FullUpdateRepresentationModel(
		imtauth::IContactInfo* contactPtr,
		imtbase::CTreeItemModel *treeItemModel) const
{
	istd::CChangeGroup changeGroup(treeItemModel);
	treeItemModel->setState("Updated");
	treeItemModel->SetData("firstName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME));
	treeItemModel->SetData("lastName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME));
	treeItemModel->SetData("nickName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME));
	//qmlRegisterType<imtbase::CTreeItemModel>("ACF", 1, 0, "TreeItemModel");

	const imtauth::IAddressProvider* addressesPtr = contactPtr->GetAddresses();
	if (addressesPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
		imtbase::CTreeItemModel *addressesModel = treeItemModel->AddTreeModel("adresses");
		for (const QByteArray& id : ids){
			const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
			if (addressPtr != nullptr){
				int index = addressesModel->InsertNewItem();
				addressesModel->SetData("addressId", id, index);
				addressesModel->SetData("country", addressPtr->GetCountry(), index);
				addressesModel->SetData("city", addressPtr->GetCity(), index);
				addressesModel->SetData("postalCode", addressPtr->GetPostalCode(), index);
				addressesModel->SetData("street", addressPtr->GetStreet(), index);
			}
		}
	}
	treeItemModel->setState("Ready");

}


void CContactInfoRepresentationDataController::FullUpdateBaseModel(
		imtauth::IContactInfo *contactPtr,
		imtbase::CTreeItemModel *treeItemModel)
{
	QList<QByteArray> keys;
	treeItemModel->GetKeys(keys);
	if (keys.isEmpty()){
		return;
	}

	m_isUpdateFromRepresentation = true;
	istd::CChangeGroup changeGroup(contactPtr);

//	contactPtr->SetEmail(EMailEdit->text());
//	contactPtr->SetBirthday(BirthdayEdit->date());
//	contactPtr->SetGenderType(imtauth::IContactInfo::GenderType(GenderCombo->currentIndex()));
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, treeItemModel->GetData("firstName").toString());
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, treeItemModel->GetData("lastName").toString());
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_NICKNAME, treeItemModel->GetData("nickName").toString());

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
				const_cast<imtauth::IAddressProvider*>(contactPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		istd::CChangeGroup addressesChangeGroup(addressesPtr);

		addressesPtr->RemoveAllAddresses();

		imtbase::CTreeItemModel *addressesModel = treeItemModel->GetTreeItemModel("adresses");
		if (addressesModel != nullptr){
			int count = addressesModel->rowCount();
			for (int i = 0; i < count; i++){

				imtauth::CAddress address;
				address.SetCountry(addressesModel->GetData("country",i).toString());
				address.SetCity(addressesModel->GetData("city",i).toString());
				address.SetPostalCode(addressesModel->GetData("postalCode",i).toInt());
				address.SetStreet(addressesModel->GetData("street",i).toString());

				addressesPtr->AddAddress(&address);
			}
		}
	}

	m_isUpdateFromRepresentation = false;
}


} // namespace imtauthgui




