#include <imtguigql/CPagesDataControllerComp.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CPagesDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
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
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "items"){
			fields = fieldList->at(i).GetFieldIds();
			isSetResponce = true;
		}
	}

	if (gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_MUTATION){
		if (m_mutationDataDelegateCompPtr.IsValid() && isSetResponce){
			if (paramList->size() <= 0){
				errorMessage = QObject::tr("Parameters of mutation is incorrect").toUtf8();
			}

			QByteArray itemData = paramList->at(0).GetFieldArgumentValue("Item").toByteArray();
			if (itemData.isEmpty()){
				errorMessage = QObject::tr("Mutation is empty").toUtf8();
			}
			else{
				imtbase::CTreeItemModel itemModel;
				itemModel.Parse(itemData);
				if (itemModel.ContainsKey("items")){
					imtbase::CTreeItemModel* items = itemModel.GetTreeItemModel("items");
					itemsModel = m_mutationDataDelegateCompPtr->UpdateBaseModelFromRepresentation(*paramList, items);
				}
			}

			if (itemsModel == nullptr){
				errorMessage = QObject::tr("Mutation is empty").toUtf8();
			}

		}
		else{
			errorMessage = QObject::tr("Incorrect query").toUtf8();
		}
	}
	else if(gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_QUERY){
		if(m_pagesDataProviderCompPtr.IsValid() && isSetResponce){
			itemsModel = m_pagesDataProviderCompPtr->GetTreeItemModel(*paramList, fields);
			if (itemsModel == nullptr){
				errorMessage = QObject::tr("Pages is empty").toUtf8();
			}
			else{
				if (m_decoratorSourceAttrPtr.IsValid()){
					for (int i = 0; i < itemsModel->GetItemsCount(); i++){
						itemsModel->SetData("DecoratorSource", *m_decoratorSourceAttrPtr, i);
					}
				}
			}
		}
		else{
			errorMessage = QObject::tr("Incorrect query").toUtf8();
		}
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


