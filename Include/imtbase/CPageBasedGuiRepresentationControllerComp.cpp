#include <imtbase/CPageBasedGuiRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtqml/IPageGuiElementModel.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CPageBasedGuiRepresentationControllerComp::GetRepresentationFromValue(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	Q_ASSERT(guiElementContainerPtr != nullptr);

	BaseClass::GetRepresentationFromValue(dataModel, representation, paramsPtr);

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = true;
	if (userInfoParamPtr.IsValid()){
		userPermissions = userInfoParamPtr->GetPermissions();

		isAdmin = userInfoParamPtr->IsAdmin();
	}

	int index = 0;

	QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();
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


} // namespace imtbase


