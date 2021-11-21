#include <imtqml/CApplicationRepresentationDataProviderCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
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



namespace imtqml
{


// public methods

CApplicationRepresentationDataProviderCompBase::CApplicationRepresentationDataProviderCompBase()
{

}

// reimplemented (imtrest::IRepresentationDataProvider)

bool CApplicationRepresentationDataProviderCompBase::GetRepresentationData(
		imtrest::IRepresentationDataProvider::Format format,
		QByteArray& representationData,
		const QList<QByteArray>& query, const imtrest::QueryParams& params)
{
	bool retVal = false;

	if (m_slaveRepresentationDataProviderCompPtr.IsValid()){
//		imtbase::CTreeItemModel *rootModel = m_slaveRepresentationDataProviderCompPtr->GetTreeItemModel(query, params);
//		if (rootModel != nullptr){
//			iser::CJsonStringWriteArchive archive(representationData);
//			retVal = rootModel->Serialize(archive);
//			delete rootModel;
//		}
	}

	return retVal;
}


bool CApplicationRepresentationDataProviderCompBase::SetRepresentationData(
			imtrest::IRepresentationDataProvider::Format format,
			QByteArray& representationData,
			const QByteArray& commandId)
{
	bool retVal = false;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(representationData);
	//	qDebug() << jsonDocument;
	if (m_slaveRepresentationDataProviderCompPtr.IsValid()
			&& format == imtrest::IRepresentationDataProvider::RF_JSON
			&& jsonDocument.isObject()){
		QJsonObject jsonObject = jsonDocument.object();

//		imtbase::CTreeItemModel* treeItemModel = m_slaveRepresentationDataProviderCompPtr->GetTreeItemModel("");

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

//		istd::IChangeable::ChangeInfoMap infoUpdate;
//		infoUpdate.insert("fromModel",true);
//		treeItemModel->SetData(infoMap.value("key").toByteArray(),infoMap.value("value"), infoMap.value("index").toInt(), infoUpdate);

	}

	return retVal;
}




} // namespace imtqml


