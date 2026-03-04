// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CGuiElementModelRepresentationControllerComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtserverapp/IGuiElementContainer.h>
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::IRepresentationController)

QByteArray CGuiElementModelRepresentationControllerComp::GetModelId() const
{
	if (m_modelIdAttrPtr.IsValid()){
		return *m_modelIdAttrPtr;
	}

	return QByteArray();
}


bool CGuiElementModelRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtserverapp::IGuiElementModel* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementModel*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CGuiElementModelRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtserverapp::IGuiElementModel* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementModel*>(&dataModel);
	if (guiElementPtr == nullptr){
		return false;
	}

	QByteArray productId;
	iprm::TParamsPtr<iprm::IIdParam> productIdParamPtr(paramsPtr, "ProductId");
	if (productIdParamPtr.IsValid()){
		productId = productIdParamPtr->GetId();
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = false;
	if (userInfoParamPtr.IsValid()){
		if (!productId.isEmpty()){
			userPermissions = userInfoParamPtr->GetPermissions(productId);
		}

		isAdmin = userInfoParamPtr->IsAdmin();
	}

	QByteArray elementId = guiElementPtr->GetElementId();

	if (!isAdmin){
		if (m_commandPermissionsProviderCompPtr.IsValid()){
			QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);
			if (m_checkPermissionCompPtr.IsValid() && !elementPermissions.isEmpty()){
				bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
				if (!result){
					return false;
				}
			}
		}
	}

	QString elementName = guiElementPtr->GetElementName();
	QString elementDescription = guiElementPtr->GetElementDescription();
	QString elementPath = guiElementPtr->GetElementItemPath();
	QString elementStatus = guiElementPtr->GetElementStatus();
	bool isEnabled = guiElementPtr->IsEnabled();
	bool isVisible = guiElementPtr->IsVisible();
	int priority = guiElementPtr->GetPriority();
	int alignment = guiElementPtr->GetAlignment();

	if (!isVisible){
		return false;
	}

	QByteArray languageId;
	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
		if (languageParamPtr.IsValid()){
			languageId = languageParamPtr->GetId();
		}
	}

	if (m_translationManagerCompPtr.IsValid() && languageId.isEmpty()){
		const iprm::IOptionsList& optionsList = m_translationManagerCompPtr->GetLanguagesInfo();
		int languageIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
		if (languageIndex >= 0){
			languageId = optionsList.GetOptionId(languageIndex);
		}
	}

	if (m_translationManagerCompPtr.IsValid()){
		QByteArray context = "Attribute";
		QString elementNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), elementName.toUtf8(), languageId, context);

		elementName = elementNameTr;
	}

	representation.SetData("id", elementId);
	representation.SetData("name", elementName);
	representation.SetData("description", elementDescription);
	representation.SetData("isEnabled", isEnabled);
	representation.SetData("visible", isVisible);
	representation.SetData("icon", elementPath);
	representation.SetData("status", elementStatus);
	representation.SetData("priority", priority);
	representation.SetData("alignment", alignment);
	representation.SetData("isToggled", false);

	const imtserverapp::IGuiElementContainer* subElementsPtr = guiElementPtr->GetSubElements();
	if (subElementsPtr != nullptr && m_representationControllerCompPtr.IsValid()){
		imtbase::CTreeItemModel* subElementsModelPtr = representation.AddTreeModel("subElements");
		if (m_representationControllerCompPtr->GetRepresentationFromDataModel(*subElementsPtr, *subElementsModelPtr, paramsPtr)){
		}
	}

	return true;
}


bool CGuiElementModelRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtserverapp


