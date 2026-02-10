// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CCommandRepresentationController.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


// public methods

CCommandRepresentationController::CCommandRepresentationController(iqt::ITranslationManager* translationManagerPtr):
	m_translationManagerPtr(translationManagerPtr)
{
}


QByteArray CCommandRepresentationController::GetModelId() const
{
	return QByteArray();
}


bool CCommandRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtserverapp::IGuiElementModel* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementModel*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CCommandRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	const imtserverapp::IGuiElementModel* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementModel*>(&dataModel);
	Q_ASSERT(guiElementPtr != nullptr);

	QByteArray elementId = guiElementPtr->GetElementId();
	QString elementDescription = guiElementPtr->GetElementDescription();
	QString elementPath = guiElementPtr->GetElementItemPath();
	QString elementStatus = guiElementPtr->GetElementStatus();
	bool isEnabled = guiElementPtr->IsEnabled();
	bool isVisible = guiElementPtr->IsVisible();
	int priority = guiElementPtr->GetPriority();
	int alignment = guiElementPtr->GetAlignment();

	QString elementName = guiElementPtr->GetElementName();

	QByteArray languageId;
	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
		if (languageParamPtr.IsValid()){
			languageId = languageParamPtr->GetId();
		}
	}

	if (m_translationManagerPtr != nullptr && languageId.isEmpty()){
		const iprm::IOptionsList& optionsList = m_translationManagerPtr->GetLanguagesInfo();
		int languageIndex = m_translationManagerPtr->GetCurrentLanguageIndex();
		if (languageIndex >= 0){
			languageId = optionsList.GetOptionId(languageIndex);
		}
	}

	if (m_translationManagerPtr != nullptr){
		QByteArray context = "Attribute";
		QString elementNameTr = iqt::GetTranslation(m_translationManagerPtr, elementName.toUtf8(), languageId, context);

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

	return true;
}


bool CCommandRepresentationController::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtserverapp


