#include <imtguigql/CSettingsControllerComp.h>


//ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CSettingsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_GET:
		return GetSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(), errorMessage);
	case OT_SAVE:
		return SaveSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(),  errorMessage);
	}

	return nullptr;
}


// protected methods
bool CSettingsControllerComp::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "Get"){
			gqlObject = fieldList->at(i);
			operationType = OT_GET;

			return true;
		}

		if (fieldList->at(i).GetId() == "Save"){
			gqlObject = fieldList->at(i);
			operationType = OT_SAVE;

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CSettingsControllerComp::GetSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (m_settingsDataProviderCompPtr.IsValid()){
		dataModel =  m_settingsDataProviderCompPtr->GetTreeItemModel(inputParams, QByteArrayList(), gqlContext);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CTreeItemModel* CSettingsControllerComp::SaveSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	if (m_mutationDataControllerCompPtr.IsValid()){
		if (inputParams.count() > 0){
			QList<imtgql::CGqlObject> params;
			QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				imtbase::CTreeItemModel settingsModel;
				settingsModel.Parse(itemData);

//				for (int i = 0; i < settingsModel.GetItemsCount(); i++){
//					imtbase::CTreeItemModel pageModel;
//					settingsModel.CopyItemDataToModel(i, &pageModel);

//					QString pageId;
//					if (pageModel.ContainsKey("Id")){
//						pageId = pageModel.GetData("Id").toString();
//					}

//					QList<imtgql::CGqlObject> params;
//					imtgql::CGqlObject gqlObject;
//					gqlObject.InsertField("Id", pageId);
//					params << gqlObject;

//					m_mutationDataControllersCompPtr[i]->UpdateBaseModelFromRepresentation(params, &pageModel, gqlContext);
//				}
				dataModel = m_mutationDataControllerCompPtr->UpdateBaseModelFromRepresentation(params, &settingsModel, gqlContext);
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


} // namespace imtgql


