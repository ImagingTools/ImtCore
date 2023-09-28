#include <imtlicgql/CAccountCollectionControllerComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/ICompanyInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

QVariant CAccountCollectionControllerComp::GetObjectInformation(
			const QByteArray& /*informationId*/,
			const QByteArray& /*objectId*/) const
{
	return QVariant();
}


imtbase::CTreeItemModel* CAccountCollectionControllerComp::GetMetaInfo(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_translationManagerCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	QByteArray languageId;
	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray accountId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	int index = dataModelPtr->InsertNewItem();

	QString modificationTime = QT_TR_NOOP("Modification Time");
	QString modificationTimeTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), modificationTime.toUtf8(), languageId, "imtlicgql::CAccountCollectionControllerComp");

	dataModelPtr->SetData("Name", modificationTimeTr, index);
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

	QString companyNameStr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Company Name"), languageId, "imtlicgql::CAccountCollectionControllerComp");

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", companyNameStr, index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString companyName = companyInfoPtr->GetName();
	children->SetData("Value", companyName);

	QString emailStr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Email"), languageId, "imtlicgql::CAccountCollectionControllerComp");

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", emailStr, index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString mail = companyInfoPtr->GetEmail();
	children->SetData("Value", mail);

	QString descriptionStr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Description"), languageId, "imtlicgql::CAccountCollectionControllerComp");

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", descriptionStr, index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString description = companyInfoPtr->GetDescription();

	children->SetData("Value", description);

	QString accountNameStr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Account Name"), languageId, "imtlicgql::CAccountCollectionControllerComp");

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", accountNameStr, index);

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
			QString& /*errorMessage*/) const
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
							elementInformation = elementMetaInfo->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime().toString("dd.MM.yyyy hh:mm:ss");
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


