#include <imtbase/CGuiElementRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
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

	iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
	QByteArray languageId;
	if (languageParamPtr.IsValid()){
		languageId = languageParamPtr->GetId();
	}

	const imtgui::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr != nullptr){
		QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();

		for (int i = 0; i < elementIds.count(); i++){
			const QByteArray elementId = elementIds[i];
			const imtgui::IGuiElementModel* guiElementPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
			if (guiElementPtr != nullptr){
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

				int index = representation.InsertNewItem();

				QString elementName = guiElementPtr->GetElementName();
				QString elementDescription = guiElementPtr->GetElementDescription();
				QString elementPath = guiElementPtr->GetElementItemPath();
				QString elementStatus = guiElementPtr->GetElementStatus();
				bool isEnabled = guiElementPtr->IsEnabled();
				bool isVisible = guiElementPtr->IsVisible();

				if (m_translationManagerCompPtr.IsValid()){
					QByteArray context = "Attribute";
					QString elementNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), elementName.toUtf8(), languageId, context);

					elementName = elementNameTr;
				}


				representation.SetData("Id", elementId, index);
				representation.SetData("Name", elementName, index);
				representation.SetData("Description", elementDescription, index);
				representation.SetData("IsEnabled", isEnabled, index);
				representation.SetData("Visible", isVisible, index);
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


