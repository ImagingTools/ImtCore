#include <imtrest/CGuiElementContainerRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtrest/IGuiElementContainer.h>
#include <imtrest/IGuiElementModel.h>


namespace imtrest
{


// protected methods

const imtrest::IRepresentationController* CGuiElementContainerRepresentationControllerComp::FindRepresentationController(const QByteArray& modelId) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return nullptr;
	}

	int count = m_representationControllersCompPtr.GetCount();

	for (int i = 0; i < count; i++){
		imtrest::IRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			QByteArray id = representationControllerPtr->GetModelId();
			if (modelId == id){
				return representationControllerPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (imtrest::IRepresentationController)

QByteArray CGuiElementContainerRepresentationControllerComp::GetModelId() const
{
	return QByteArray();
}


bool CGuiElementContainerRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtrest::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtrest::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CGuiElementContainerRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtrest::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtrest::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr == nullptr) {
		return false;
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = false;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();
		isAdmin = userInfoParamPtr->IsAdmin();
	}

	QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		const imtrest::IGuiElementModel* guiElementModelPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
		if (guiElementModelPtr != nullptr){
			if (!isAdmin){
				if (m_commandPermissionsProviderCompPtr.IsValid() && !elementId.isEmpty()){
					QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);

					if (m_checkPermissionCompPtr.IsValid()){
						bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
						if (!result){
							continue;
						}
					}
				}
			}

			const imtrest::IRepresentationController* representationControllerPtr = FindRepresentationController(elementId);
			if (representationControllerPtr != nullptr){
				imtbase::CTreeItemModel guiElementRepresentationModel;
				if (representationControllerPtr->GetRepresentationFromDataModel(*guiElementModelPtr, guiElementRepresentationModel, paramsPtr)){
					int index = representation.InsertNewItem();
					representation.CopyItemDataFromModel(index, &guiElementRepresentationModel);
				}
			}
		}
	}

	if (m_slaveRepresentationControllerCompPtr.IsValid()){
		bool ok = m_slaveRepresentationControllerCompPtr->GetRepresentationFromDataModel(*guiElementContainerPtr, representation, paramsPtr);
		if (!ok){
			return false;
		}
	}

	return true;
}


bool CGuiElementContainerRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtrest


