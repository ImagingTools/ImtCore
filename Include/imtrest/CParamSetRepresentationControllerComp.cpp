#include <imtrest/CParamSetRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserInfo.h>


namespace imtrest
{


// protected methods

const imtrest::IRepresentationController* CParamSetRepresentationControllerComp::FindSubController(const QByteArray& parameterId) const
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


// reimplemented (imtrest::CObjectRepresentationControllerCompBase)

bool CParamSetRepresentationControllerComp::GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	Q_ASSERT(paramsSetPtr != nullptr);

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	bool isAdmin = true;
	imtauth::IUserInfo::FeatureIds userPermissions;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();

		isAdmin = userInfoParamPtr->IsAdmin();
		if (!isAdmin){
			bool result = CheckPermissions(userPermissions, *m_objectIdAttrPtr);
			if (!result){
				return false;
			}
		}
	}

	imtbase::CTreeItemModel* parametersRepresentationPtr = representation.AddTreeModel("Parameters");
	Q_ASSERT(parametersRepresentationPtr != nullptr);

	iprm::IParamsSet::Ids parameterIds = paramsSetPtr->GetParamIds();

	QByteArrayList ids = parameterIds.values();
	std::sort(ids.begin(), ids.end());

	for (const QByteArray& paramId : ids){
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
				istd::TDelPtr<imtbase::CTreeItemModel> parameterRepresentationPtr = new imtbase::CTreeItemModel();
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

	return true;
}


// reimplemented (IRepresentationController)

bool CParamSetRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		return true;
	}

	return false;
}


bool CParamSetRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		const imtbase::CTreeItemModel* parametersModelPtr = representation.GetTreeItemModel("Parameters");
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
						const imtbase::CTreeItemModel* representationModelPtr = parametersModelPtr->GetModelFromItem(i);

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


} // namespace imtrest


