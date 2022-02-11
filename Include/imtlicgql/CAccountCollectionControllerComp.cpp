#include <imtlicgql/CAccountCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/CAccountInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CAccountCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;

	if (m_objectCollectionCompPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
		if (informationId == QByteArray("AccountName")){
			return metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME);
		}
		else if (informationId == QByteArray("Email")){
			return metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_EMAIL);
		}
		else if (informationId == QByteArray("AccountType")){
			return metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE);
		}
		else if (informationId == QByteArray("AccountDescription")){
			return metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION);
		}
	}

	return QVariant();
}


istd::IChangeable* CAccountCollectionControllerComp::CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray& objectId,
																	 QString& name, QString& description, QString &errorMessage) const
{
	if (inputParams.isEmpty()){
		return nullptr;
	}
	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		imtauth::CAccountInfo *accountInfo = new imtauth::CAccountInfo();
		imtbase::CTreeItemModel itemModel;
		itemModel.Parse(itemData);

		if (itemModel.ContainsKey("AccountName")){
			name = itemModel.GetData("AccountName").toString();
			objectId = itemModel.GetData("AccountName").toByteArray();
		}

		if (itemModel.ContainsKey("AccountDescription")){
			description = itemModel.GetData("AccountDescription").toString();
		}

		accountInfo->SetAccountName(name);
		accountInfo->SetAccountDescription(description);

		imtauth::CContactInfo contactInfo;

		if (itemModel.ContainsKey("Email")) {
			QString email = itemModel.GetData("Email").toString();
			contactInfo.SetEmail(email);
		}

		if (itemModel.ContainsKey("LastName")) {
			QString lastName = itemModel.GetData("LastName").toString();
			contactInfo.SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, lastName);
		}

		if (itemModel.ContainsKey("FirstName")) {
			QString firstName = itemModel.GetData("FirstName").toString();
			contactInfo.SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, firstName);
		}

		accountInfo->SetAccountOwner(contactInfo);

		return accountInfo;
	}

	return nullptr;
}

imtbase::CTreeItemModel* CAccountCollectionControllerComp::GetMetaInfo(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* metaInfoModel = nullptr;
	imtbase::CTreeItemModel* childs = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{

		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;

		QByteArray accountId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;

		m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr);

		const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(dataPtr.GetPtr());

		const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();

		int index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Last Name", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
		childs->SetData("Value", lastName);

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "First Name", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
		childs->SetData("Value", firstName);

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Email", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString mail = ownerPtr->GetMail();
		childs->SetData("Value", mail);

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Description", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString description = accountInfoPtr->GetAccountDescription();

		childs->SetData("Value", description);

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Account Name", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString name = accountInfoPtr->GetAccountName();

		childs->SetData("Value", name);

		index = metaInfoModel->InsertNewItem();
		metaInfoModel->SetData("Name", "Account Type", index);
		childs = metaInfoModel->AddTreeModel("Childs", index);

		imtauth::IAccountInfo::AccountType type = accountInfoPtr->GetAccountType();

		if (type == imtauth::IAccountInfo::AT_COMPANY){
			childs->SetData("Value", "Company");
		}
		else{
			childs->SetData("Value", "Private");
		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


