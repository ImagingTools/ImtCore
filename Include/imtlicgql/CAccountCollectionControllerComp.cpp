#include <imtlicgql/CAccountCollectionControllerComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ImtCore includes
#include <imtauth/CAccountInfo.h>
#include <imtauth/ICompanyInfo.h>


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

	QString companyName = companyInfoPtr->GetCompanyName();
	children->SetData("Value", companyName);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Email"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString mail = companyInfoPtr->GetMail();
	children->SetData("Value", mail);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Description"), index);
	children = dataModelPtr->AddTreeModel("Children", index);

	QString description = companyInfoPtr->GetAccountDescription();

	children->SetData("Value", description);

	index = dataModelPtr->InsertNewItem();
	dataModelPtr->SetData("Name", QT_TR_NOOP("Account Name"), index);

	children = dataModelPtr->AddTreeModel("Children", index);

	QString name = companyInfoPtr->GetAccountName();

	children->SetData("Value", name);

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


