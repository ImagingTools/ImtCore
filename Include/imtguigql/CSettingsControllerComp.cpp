#include <imtguigql/CSettingsControllerComp.h>


//ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CHierarchicalItemModelPtr CSettingsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return imtbase::CHierarchicalItemModelPtr();
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return imtbase::CHierarchicalItemModelPtr();
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_GET:
		return GetSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(), errorMessage);
	case OT_SAVE:
		return SaveSettings(*inputParamsPtr, gqlObject, gqlRequest.GetGqlContext(),  errorMessage);
	}

	return imtbase::CHierarchicalItemModelPtr();
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


imtbase::CHierarchicalItemModelPtr CSettingsControllerComp::GetSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;

	if (m_settingsDataProviderCompPtr.IsValid()){
		dataModel =  m_settingsDataProviderCompPtr->GetRepresentation(inputParams, QByteArrayList(), gqlContext);
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
}


imtbase::CHierarchicalItemModelPtr CSettingsControllerComp::SaveSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		const imtgql::IGqlContext* gqlContext,
		QString &errorMessage) const
{
	if (m_mutationDataControllerCompPtr.IsValid()){
		if (inputParams.count() > 0){
			QList<imtgql::CGqlObject> params;
			QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();

			if (!itemData.isEmpty()){
				imtbase::CHierarchicalItemModelPtr rootModel(new imtbase::CTreeItemModel());

				bool retVal = false;
				imtbase::CTreeItemModel settingsModel;
				if (settingsModel.CreateFromJson(itemData)){

					for (int i = 0; i < settingsModel.GetItemsCount(); i++){
						if (settingsModel.ContainsKey("Id", i)){
							QByteArray paramId =  settingsModel.GetData("Id", i).toByteArray();
						}
					}

					settingsModel.GetTreeItemModel("Elements");

					retVal = m_mutationDataControllerCompPtr->UpdateModelFromRepresentation(params, &settingsModel, gqlContext);
				}

				rootModel->SetData("SaveStatus", retVal);

				return rootModel;
			}
		}
	}

	return imtbase::CHierarchicalItemModelPtr();
}


} // namespace imtgql


