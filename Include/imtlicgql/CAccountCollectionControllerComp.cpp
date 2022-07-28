#include <imtlicgql/CAccountCollectionControllerComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ImtCore includes
#include <imtauth/CAccountInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CAccountCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (metaInfoPtr.IsValid()){
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
		errorMessage = QT_TR_NOOP("Internal error");
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		metaInfoModel = new imtbase::CTreeItemModel();

		idoc::MetaInfoPtr metaInfoPtr;

		QByteArray accountId = GetObjectIdFromInputParams(inputParams);

		imtbase::IObjectCollection::DataPtr dataPtr;

		const QTranslator* translatorPtr = nullptr;
		if (m_translationManagerCompPtr.IsValid()){
//			QByteArray languageId = GetLanguageIdFromInputParams(inputParams);
			QByteArray languageId;
			languageId = "en_US";
			if (languageId.isEmpty()){
				int currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
				if (currentIndex >= 0){
					translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
				}
			}
		}

		if (!m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
			errorMessage = QT_TR_NOOP("Unable to load an object data");
			return nullptr;
		}

		const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(dataPtr.GetPtr());
		if (accountInfoPtr == nullptr){
			if (translatorPtr != nullptr){
				errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Unable to get an account info");
			}
			else{
				errorMessage = QT_TR_NOOP("Unable to get an account info");
			}

			return nullptr;
		}

		const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();

		if (ownerPtr == nullptr){
			if (translatorPtr != nullptr){
				errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Unable to get an account owner");
			}
			else{
				errorMessage = QT_TR_NOOP("Unable to get an account owner");
			}

			return nullptr;
		}

		int index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Last Name");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Last Name"), index);
		}
		childs = metaInfoModel->AddTreeModel("Childs", index);
		QString lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
		childs->SetData("Value", lastName);

		index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "First Name");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("First Name"), index);
		}
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
		childs->SetData("Value", firstName);

		index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Email");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Email"), index);
		}
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString mail = ownerPtr->GetMail();
		childs->SetData("Value", mail);

		index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Description");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Description"), index);
		}
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString description = accountInfoPtr->GetAccountDescription();

		childs->SetData("Value", description);

		index = metaInfoModel->InsertNewItem();
		if (translatorPtr != nullptr){
			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Account Name");
		}
		else{
			metaInfoModel->SetData("Name", QT_TR_NOOP("Account Name"), index);
		}
		childs = metaInfoModel->AddTreeModel("Childs", index);

		QString name = accountInfoPtr->GetAccountName();

		childs->SetData("Value", name);

//		index = metaInfoModel->InsertNewItem();
//		if (translatorPtr != nullptr){
//			errorMessage = translatorPtr->translate("imtlicgql::CAccountCollectionControllerComp", "Account Type");
//		}
//		else{
//			metaInfoModel->SetData("Name", QT_TR_NOOP("Account Type"), index);
//		}
//		childs = metaInfoModel->AddTreeModel("Childs", index);

//		imtauth::IAccountInfo::AccountType type = accountInfoPtr->GetAccountType();

//		if (type == imtauth::IAccountInfo::AT_COMPANY){
//			childs->SetData("Value", "Company");
//		}
//		else{
//			childs->SetData("Value", "Private");
//		}

		dataModel->SetExternTreeModel("metaInfo", metaInfoModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtlicgql


