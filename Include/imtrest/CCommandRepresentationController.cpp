#include <imtrest/CCommandRepresentationController.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtrest/IGuiElementModel.h>


namespace imtrest
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
	const imtrest::IGuiElementModel* guiElementPtr = dynamic_cast<const imtrest::IGuiElementModel*>(&dataModel);
	if (guiElementPtr != nullptr){
		return true;
	}

	return false;
}


bool CCommandRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	const imtrest::IGuiElementModel* guiElementPtr = dynamic_cast<const imtrest::IGuiElementModel*>(&dataModel);
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
		QString elementNameTr = imtbase::GetTranslation(m_translationManagerPtr, elementName.toUtf8(), languageId, context);

		elementName = elementNameTr;
	}

	representation.SetData("Id", elementId);
	representation.SetData("Name", elementName);
	representation.SetData("Description", elementDescription);
	representation.SetData("IsEnabled", isEnabled);
	representation.SetData("Visible", isVisible);
	representation.SetData("Icon", elementPath);
	representation.SetData("Status", elementStatus);
	representation.SetData("Priority", priority);
	representation.SetData("Alignment", alignment);

	return true;
}


bool CCommandRepresentationController::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtrest


