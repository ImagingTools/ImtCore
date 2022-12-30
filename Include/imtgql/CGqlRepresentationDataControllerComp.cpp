#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlRepresentationDataControllerComp::IsRequestSupported(const CGqlRequest& gqlRequest) const
{
	const QList<CGqlObject>* fieldsPtr = gqlRequest.GetFields();
	if (fieldsPtr == nullptr){
		return false;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId == *m_getCommandIdAttrPtr || commandId == *m_setCommandIdAttrPtr){
		for (int i = 0; i < m_modelIdsAttrPtr.GetCount(); i++){
			QByteArray modelId = m_modelIdsAttrPtr[i];

			for (int j = 0; j < fieldsPtr->count(); j++){
				QByteArrayList fieldIds = fieldsPtr->at(j).GetFieldIds();
				if (fieldIds.contains(modelId)){
					return true;
				}
			}
		}
	}

	return false;
}


// protected methods

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_representationControllerCompPtr.IsValid() && m_dataModelCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		imtbase::CTreeItemModel* representationPtr = rootModelPtr->AddTreeModel("data");
		Q_ASSERT(representationPtr != nullptr);

		bool result = m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_dataModelCompPtr, *representationPtr);
		if (result){
			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


bool CGqlRepresentationDataControllerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (m_representationControllerCompPtr.IsValid() && m_dataModelCompPtr.IsValid()){
		const QList<imtgql::CGqlObject>* inputParams = request.GetParams();
		if (inputParams->count() > 0){
			QByteArray itemData = inputParams->at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				imtbase::CTreeItemModel settingsModel;
				if (settingsModel.CreateFromJson(itemData)){
					istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
					bool result = m_representationControllerCompPtr->GetDataModelFromRepresentation(settingsModel, *m_dataModelCompPtr);
					if (result){
						rootModelPtr->SetData("SaveStatus", result);

						rootModelPtr.PopPtr();

						return true;
					}
				}
			}
		}
	}

	return false;
}


// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId == *m_getCommandIdAttrPtr){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	else if (commandId == *m_setCommandIdAttrPtr){
		const QList<CGqlObject>* paramsPtr = gqlRequest.GetParams();
		if (paramsPtr != nullptr){
			QByteArray itemData = paramsPtr->at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
				if (representationPtr->CreateFromJson(itemData)){
					bool result = UpdateModelFromRepresentation(gqlRequest, representationPtr.GetPtr());
					if (result){
						return representationPtr.PopPtr();
					}
				}
			}
		}
	}

	Q_ASSERT(false);

	return nullptr;
}


} // namespace imtgql


