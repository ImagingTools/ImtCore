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
		else if (informationId == QByteArray("OwnerMail")){
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


imtbase::CTreeItemModel* CAccountCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray packageId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	int index = dataModelPtr->InsertNewItem();

	dataModelPtr->SetData("Name", QT_TR_NOOP("Modification Time"), index);
	imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(packageId);
	if (metaInfo.IsValid()){
		QString date = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
		children->SetData("Value", date);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(packageId, dataPtr)){
		errorMessage = QT_TR_NOOP("Unable to load an object data");

		return nullptr;
	}

	imtbase::IObjectCollection* licensePtr = dynamic_cast<imtbase::IObjectCollection*>(dataPtr.GetPtr());
	if (licensePtr != nullptr){
		QByteArrayList licenseCollectionIds = licensePtr->GetElementIds().toList();
		index = dataModelPtr->InsertNewItem();

		dataModelPtr->SetData("Name", "Licenses", index);
		children = dataModelPtr->AddTreeModel("Children", index);
		int childIndex;
		for (const QByteArray& licenseCollectionId : licenseCollectionIds){
			childIndex = children->InsertNewItem();
			QString licenseName = licensePtr->GetElementInfo(licenseCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString();
			QString value = licenseName + " (" + licenseCollectionId + ")";
			children->SetData("Value", value, childIndex);
		}
	}

	const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(dataPtr.GetPtr());
	if (accountInfoPtr == nullptr){
		return nullptr;
	}

	const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();
	if (ownerPtr == nullptr){
		return nullptr;
	}

	children = dataModelPtr->AddTreeModel("Children", index);
	QString lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
	children->SetData("Value", lastName);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("First Name"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
	children->SetData("Value", firstName);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Email"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString mail = ownerPtr->GetMail();
	children->SetData("Value", mail);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Description"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString description = accountInfoPtr->GetAccountDescription();

	children->SetData("Value", description);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Account Name"), index);

	children = dataModelPtr->AddTreeModel("Children", index);

	QString name = accountInfoPtr->GetAccountName();

	children->SetData("Value", name);

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


