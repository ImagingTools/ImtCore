#include <imtgui/CGuiElementContainerRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtgui/IGuiElementContainer.h>


namespace imtgui
{


// protected methods

const imtbase::IRepresentationController* CGuiElementContainerRepresentationControllerComp::FindRepresentationController(const QByteArray& modelId) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return nullptr;
	}

	int count = m_representationControllersCompPtr.GetCount();

	for (int i = 0; i < count; i++){
		imtbase::IRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			QByteArray id = representationControllerPtr->GetModelId();
			if (modelId == id){
				return representationControllerPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (imtbase::IRepresentationController)

QByteArray CGuiElementContainerRepresentationControllerComp::GetModelId() const
{
	return QByteArray();
}


bool CGuiElementContainerRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
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

	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
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
		const imtgui::IGuiElementModel* guiElementModelPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
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

			const imtbase::IRepresentationController* representationControllerPtr = FindRepresentationController(elementId);
			if (representationControllerPtr != nullptr){
				imtbase::CTreeItemModel guiElementRepresentationModel;
				if (representationControllerPtr->GetRepresentationFromDataModel(*guiElementModelPtr, guiElementRepresentationModel, paramsPtr)){
					int index = representation.InsertNewItem();
					representation.CopyItemDataFromModel(index, &guiElementRepresentationModel);
				}
			}
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


} // namespace imtgui


