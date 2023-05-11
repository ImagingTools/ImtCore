#include <imtlicgql/CAccountCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/ICompanyInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

QVariant CAccountCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
//	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
//	if (metaInfoPtr.IsValid()){
//		if (informationId == QByteArray("Name")){
//			return metaInfoPtr->GetMetaInfo(imtauth::ICompanyInfo::MIT_ACCOUNT_NAME);
//		}
//		else if (informationId == QByteArray("Mail")){
//			return metaInfoPtr->GetMetaInfo(imtauth::ICompanyInfo::MIT_MAIL);
//		}
//		else if (informationId == QByteArray("AccountType")){
//			return metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE);
//		}
//		else if (informationId == QByteArray("Description")){
//			return metaInfoPtr->GetMetaInfo(imtauth::ICompanyInfo::MIT_ACCOUNT_DESCRIPTION);
//		}
//	}

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

	QByteArray accountId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	int index = dataModelPtr->InsertNewItem();

	dataModelPtr->SetData("Name", QT_TR_NOOP("Modification Time"), index);
	imtbase::CTreeItemModel* children = dataModelPtr->AddTreeModel("Children", index);

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(accountId);
	if (metaInfo.IsValid()){
		QString date = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
		children->SetData("Value", date);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_objectCollectionCompPtr->GetObjectData(accountId, dataPtr)){
		errorMessage = QT_TR_NOOP("Unable to load an object data");

		return nullptr;
	}

	const imtauth::ICompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(dataPtr.GetPtr());
	if (companyInfoPtr == nullptr){
		return nullptr;
	}

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Company Name"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString companyName = companyInfoPtr->GetName();
	children->SetData("Value", companyName);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Email"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString mail = companyInfoPtr->GetEmail();
	children->SetData("Value", mail);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Description"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString description = companyInfoPtr->GetDescription();

	children->SetData("Value", description);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Account Name"), index);

	children = dataModelPtr->AddTreeModel("Children", index);

	QString name = companyInfoPtr->GetName();

	children->SetData("Value", name);

	return rootModelPtr.PopPtr();
}


bool CAccountCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;
	QByteArray collectionId = objectCollectionIterator->GetObjectId();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty()){
		const imtauth::ICompanyInfo* companyInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (objectCollectionIterator->GetObjectData(userDataPtr)){
			companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(userDataPtr.GetPtr());
		}

		if (companyInfoPtr != nullptr){
			idoc::MetaInfoPtr elementMetaInfo = objectCollectionIterator->GetDataMetaInfo();

//			imtauth::CAddress address = companyInfoPtr->GetAddress();
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "Id"){
					elementInformation = QString(collectionId);
				}
				else if(informationId == "Name"){
					elementInformation = companyInfoPtr->GetName();
				}
				else if(informationId == "Description"){
					elementInformation = companyInfoPtr->GetDescription();
				}
				else if(informationId == "Email"){
					elementInformation = companyInfoPtr->GetEmail();
				}
				else{
					if (elementMetaInfo.IsValid()){
						if (informationId == QByteArray("Added")){
							elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
									.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
						}
						else if (informationId == QByteArray("LastModified")){
							elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
									.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
						}
					}
				}

				if(elementInformation.isNull()){
					elementInformation = GetObjectInformation(informationId, collectionId);
				}
				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}

		}

		return true;
	}

	return false;
}


} // namespace imtlicgql


