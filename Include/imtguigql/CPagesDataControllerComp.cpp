#include <imtguigql/CPagesDataControllerComp.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CPagesDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (gqlRequest.GetCommandId() != *m_modelIdCompPtr){
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

	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "items"){
			fields = fieldList->at(i).GetFieldIds();
			isSetResponce = true;
		}
	}

	if(m_pagesDataProviderCompPtr.IsValid() && isSetResponce){
		itemsModel = m_pagesDataProviderCompPtr->GetTreeItemModel(*paramList, fields);
		if (itemsModel == nullptr){
			errorMessage = QObject::tr("Pages is empty").toUtf8();
		}
	}
	else{
		errorMessage = QObject::tr("Incorrect query").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");

		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();

		dataModel->SetExternTreeModel("items", itemsModel);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtgql


