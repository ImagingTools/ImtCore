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
		objectId = itemModel.GetData("Id").toByteArray();
		description = itemModel.GetData("AccountDescription").toString();
		name = itemModel.GetData("AccountName").toString();
		accountInfo->SetAccountName(name);
		accountInfo->SetAccountDescription(description);
		return accountInfo;
	}
	return nullptr;

}


} // namespace imtlicgql


