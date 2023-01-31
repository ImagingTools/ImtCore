#include <imtbase/CPageBasedGuiRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtqml/IPageGuiElementModel.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CPageBasedGuiRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = true;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();

		isAdmin = userInfoParamPtr->IsAdmin();
	}

	BaseClass::GetRepresentationFromDataModel(dataModel, representation, paramsPtr);

	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr != nullptr){
		QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();

		int index = 0;
		for (int i = 0; i < elementIds.count(); i++){
			const QByteArray elementId = elementIds[i];
			const imtgui::IGuiElementModel* guiElementPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
			if (guiElementPtr != nullptr){
				if (!isAdmin){
					if (m_commandPermissionsProviderCompPtr.IsValid()){
						QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);
						if (m_checkPermissionCompPtr.IsValid()){
							bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
							if (!result){
								continue;
							}
						}
					}
				}

				const imtqml::IPageGuiElementModel* pageGuiElementPtr = dynamic_cast<const imtqml::IPageGuiElementModel*>(guiElementPtr);
				if (pageGuiElementPtr != nullptr){
					QString pageQmlItemFilePath = pageGuiElementPtr->GetPageQmlItemFilePath();
					QString startSourceItem = pageGuiElementPtr->GetStartSourceItem();

					representation.SetData("Source", pageQmlItemFilePath, index);
					representation.SetData("StartItem", startSourceItem, index);

					index++;
				}
			}
		}

		return true;
	}

	return false;
}


} // namespace imtbase


