#include <imtbase/CGuiElementRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CGuiElementRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CGuiElementRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!m_checkPermissionCompPtr.IsValid() || !m_commandPermissionsProviderCompPtr.IsValid()){
		return false;
	}

	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();
	}

	bool isAdmin = userInfoParamPtr->IsAdmin();

	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr != nullptr){
		QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();

		for (int i = 0; i < elementIds.count(); i++){
			const QByteArray elementId = elementIds[i];
			const imtgui::IGuiElementModel* guiElementPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
			if (guiElementPtr != nullptr){
				if (!isAdmin){
					QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);

					bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
					if (!result){
						continue;
					}
				}

				int index = representation.InsertNewItem();

				QString elementName = guiElementPtr->GetElementName();
				QString elementDescription = guiElementPtr->GetElementDescription();
				QString elementPath = guiElementPtr->GetElementItemPath();
				QString elementStatus = guiElementPtr->GetElementStatus();
				bool isEnabled = guiElementPtr->IsEnabled();

				representation.SetData("Id", elementId, index);
				representation.SetData("Name", elementName, index);
				representation.SetData("Description", elementDescription, index);
				representation.SetData("IsEnabled", isEnabled, index);
				representation.SetData("Visible", true, index);
				representation.SetData("Icon", elementPath, index);
				representation.SetData("Status", elementStatus, index);
			}
		}

		return true;
	}

	return false;
}


bool CGuiElementRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return true;
}


} // namespace imtbase


