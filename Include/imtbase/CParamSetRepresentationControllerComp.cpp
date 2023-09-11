#include <imtbase/CParamSetRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CParamSetRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		return true;
	}

	return false;
}


bool CParamSetRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	bool isAdmin = true;
	imtauth::IUserInfo::FeatureIds userPermissions;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();

		isAdmin = userInfoParamPtr->IsAdmin();
		if (!isAdmin){
			bool result = CheckPermissions(userPermissions, *m_paramIdAttrPtr);
			if (!result){
				return false;
			}
		}
	}

	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		representation.SetData("Id", *m_paramIdAttrPtr);

		QString paramName = *m_paramNameAttrPtr;
		representation.SetData("Name", *m_paramNameAttrPtr);

		if (m_qmlPathAttrPtr.IsValid()){
			representation.SetData("Source", *m_qmlPathAttrPtr);
		}

		iprm::IParamsSet::Ids parameterIds = paramsSetPtr->GetParamIds();

		CTreeItemModel* parametersRepresentationPtr = representation.AddTreeModel("Parameters");
		Q_ASSERT(parametersRepresentationPtr != nullptr);

		for (const QByteArray& paramId : parameterIds){
			if (!paramId.contains('/')){
				if (!isAdmin){
					bool result = CheckPermissions(userPermissions, paramId);
					if (!result){
						continue;
					}
				}

				const iser::ISerializable* paramPtr = paramsSetPtr->GetParameter(paramId);
				Q_ASSERT(paramPtr != nullptr);

				const IRepresentationController* subControllerPtr = FindSubController(paramId);
				if (subControllerPtr != nullptr){
					istd::TDelPtr<CTreeItemModel> parameterRepresentationPtr = new imtbase::CTreeItemModel();
					if (subControllerPtr->GetRepresentationFromDataModel(*paramPtr, *parameterRepresentationPtr, paramsPtr)){
						int index = parametersRepresentationPtr->InsertNewItem();

						parametersRepresentationPtr->CopyItemDataFromModel(index, parameterRepresentationPtr.PopPtr());
					}
					else{
						return false;
					}
				}
			}
		}
	}

	return true;
}


bool CParamSetRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		const CTreeItemModel* parametersModelPtr = representation.GetTreeItemModel("Parameters");
		if (parametersModelPtr != nullptr){
			for (int i = 0; i < parametersModelPtr->GetItemsCount(); i++){
				QByteArray parameterId;
				if (parametersModelPtr->ContainsKey("Id", i)){
					parameterId = parametersModelPtr->GetData("Id", i).toByteArray();
				}

				if (!parameterId.isEmpty()){
					const IRepresentationController* subControllerPtr = FindSubController(parameterId);
					if (subControllerPtr != nullptr){
						istd::IChangeable* modelPtr = paramsSetPtr->GetEditableParameter(parameterId);
						const CTreeItemModel* representationModelPtr = parametersModelPtr->GetModelFromItem(i);

						if (modelPtr != nullptr && representationModelPtr != nullptr){
							bool result = subControllerPtr->GetDataModelFromRepresentation(*representationModelPtr, *modelPtr);
							if (!result){
								return false;
							}
						}
					}
				}
			}
		}
	}

	return true;
}


const IRepresentationController* CParamSetRepresentationControllerComp::FindSubController(const QByteArray& parameterId) const
{
	for (int i = 0; i < m_paramRepresentationControllersCompPtr.GetCount(); i++){
		const IRepresentationController* subControllerPtr = m_paramRepresentationControllersCompPtr[i];
		if (subControllerPtr != nullptr){
			if (subControllerPtr->GetModelId() == parameterId){
				return subControllerPtr;
			}
		}
	}

	return nullptr;
}


} // namespace imtbase


