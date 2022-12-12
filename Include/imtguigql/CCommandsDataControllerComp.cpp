#include <imtguigql/CCommandsDataControllerComp.h>


// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CCommandsDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	int count = fieldList->count();

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;
	QByteArray commandsId;

	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "items"){
			fields = fieldList->at(i).GetFieldIds();
			if (paramList != nullptr && !paramList->isEmpty()){
				commandsId = paramList->at(0).GetFieldArgumentValue(imtqml::CPageDataEnumProviderComp::ID).toByteArray();
			}
			isSetResponce = true;
		}
	}

	if(m_commandsDataProviderCompPtr.IsValid() && isSetResponce){
		for (int index = 0; index < m_commandsDataProviderCompPtr.GetCount(); index++){
			if (m_commandsDataProviderCompPtr[index]->GetModelId() == commandsId){
				itemsModel = m_commandsDataProviderCompPtr[index]->GetRepresentation(*paramList, fields, gqlRequest.GetGqlContext());
			}
		}
		if (itemsModel == nullptr){
			isSetResponce = false;
		}
	}
	else{
		errorMessage = QObject::tr("Incorrect qyery").toUtf8();
	}

	if (!isSetResponce){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {

		dataModel = new imtbase::CTreeItemModel();
		dataModel->SetExternTreeModel("items", itemsModel);
		imtbase::CTreeItemModel* infoModel = new imtbase::CTreeItemModel();
		infoModel->SetData("CommandsModelId", commandsId);
		dataModel->SetExternTreeModel("information", infoModel);

	}
	rootModel->SetExternTreeModel("data", dataModel);
	return rootModel;
}


} // namespace imtgql


