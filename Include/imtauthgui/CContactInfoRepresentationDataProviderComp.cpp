#include <imtauthgui/CContactInfoRepresentationDataProviderComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>
#include <imtbase/CTreeItemModel.h>



namespace imtauthgui
{


// public methods

CContactInfoRepresentationDataProviderComp::CContactInfoRepresentationDataProviderComp()
{

}

// reimplemented (imtrest::IRepresentationDataProvider)

bool CContactInfoRepresentationDataProviderComp::GetRepresentationData(
			imtrest::IRepresentationDataProvider::Format format,
			QByteArray& representationData,
			const QList<QByteArray>& query,
			const imtrest::QueryParams& params)
{
	bool retVal = false;

	if (	m_itemBasedRepresentationDataProvider.IsValid() &&
			format == imtrest::IRepresentationDataProvider::RF_JSON &&
			!query.isEmpty() &&
			query[0] == "__ContactInfo__"){
//		imtbase::CTreeItemModel *treeItemModel = m_itemBasedRepresentationDataProvider->GetTreeItemModel(query, params);

//		iser::CJsonStringWriteArchive archive(representationData);

//		retVal = treeItemModel->Serialize(archive);
	}

	return retVal;
}


bool CContactInfoRepresentationDataProviderComp::SetRepresentationData(
			imtrest::IRepresentationDataProvider::Format format,
			QByteArray& representationData,
			const QByteArray& commandId)
{
	bool retVal = false;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(representationData);
	if (	m_itemBasedRepresentationDataProvider.IsValid() &&
			format == imtrest::IRepresentationDataProvider::RF_JSON &&
			commandId == "__ContactInfo__" &&
			jsonDocument.isObject()){

		QList<QByteArray> query;
		imtrest::QueryParams params;
//		imtbase::CTreeItemModel* treeItemModel = m_itemBasedRepresentationDataProvider->GetTreeItemModel(query, params);

//		QJsonObject jsonObject = jsonDocument.object();
//		if (jsonObject.value("key").toString() == "addresses"){
//			imtbase::CTreeItemModel* addressesModel = treeItemModel->GetTreeItemModel("addresses",jsonObject.value("index0").toInt());
//			if (addressesModel != nullptr){
//				QJsonArray jsonAddresses = jsonObject.value("addresses").toArray();
//				for(int i = 0; i < jsonAddresses.count(); i++){
//					QJsonObject jsonAddress = jsonAddresses[i].toObject();
//					if (jsonAddress.isEmpty()){
//						for(QString key : jsonObject.keys()){
//							treeItemModel->SetData(key.toLatin1(),jsonAddress.value(key).toVariant(),jsonAddress.value("index").toInt());
//						}
//					}
//				}
//			}
//		}
//		else{
//			treeItemModel->SetState("Updated");
//			for(QString key : jsonObject.keys()){
//				treeItemModel->SetData(key.toLatin1(),jsonObject.value(key).toVariant());

//				retVal = true;
//			}

//			treeItemModel->SetState("Ready");
//		}
	}

	return retVal;
}


} // namespace imtauthgui


