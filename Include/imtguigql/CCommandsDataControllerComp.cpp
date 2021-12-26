#include "CCommandsDataControllerComp.h"

// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtguigql
{


//// reimplemented (imtgql::IGqlRepresentationDataController)


imtbase::CTreeItemModel* CCommandsDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{

	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	int count = fieldList->count();

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;
	QByteArray pageId;

	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "items"){
			fields = fieldList->at(i).GetFieldIds();
			if (paramList != nullptr && !paramList->isEmpty()){
				pageId = paramList->at(0).GetFieldArgumentValue(imtqml::CPageDataEnumProviderComp::ID).toByteArray();
			}
			isSetResponce = true;
		}
	}

	if(m_commandsDataProviderCompPtr.IsValid() && isSetResponce){
		for (int index = 0; index < m_commandsDataProviderCompPtr.GetCount(); index++){
			if (m_commandsDataProviderCompPtr[index]->GetModelId() == pageId){
				itemsModel = m_commandsDataProviderCompPtr[index]->GetTreeItemModel(*paramList, fields);
			}
		}
		if (itemsModel == nullptr){
			errorMessage = QObject::tr("Commands is empty").toUtf8();
		}
	}
	else{
		errorMessage = QObject::tr("Incorrect qyery").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {

		dataModel = new imtbase::CTreeItemModel();
		dataModel->SetExternTreeModel("items", itemsModel);

	}
	rootModel->SetExternTreeModel("data", dataModel);
	return rootModel;
}


} // namespace imtgql


