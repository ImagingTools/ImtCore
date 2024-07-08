#include <imtgui/CGuiElementModelRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtgui
{


// protected methods

// reimplemented (imtbase::IRepresentationController)

QByteArray CGuiElementModelRepresentationControllerComp::GetModelId() const
{
	if (m_modelIdAttrPtr.IsValid()){
		return *m_modelIdAttrPtr;
	}

	return QByteArray();
}


bool CGuiElementModelRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtgui::IGuiElementModel* guiElementPtr = dynamic_cast<const imtgui::IGuiElementModel*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CGuiElementModelRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtgui::IGuiElementModel* guiElementPtr = dynamic_cast<const imtgui::IGuiElementModel*>(&dataModel);
	if (guiElementPtr == nullptr) {
		return false;
	}

	QByteArray elementId = guiElementPtr->GetElementId();
	QString elementDescription = guiElementPtr->GetElementDescription();
	QString elementPath = guiElementPtr->GetElementItemPath();
	QString elementStatus = guiElementPtr->GetElementStatus();
	bool isEnabled = guiElementPtr->IsEnabled();
	bool isVisible = guiElementPtr->IsVisible();
	int priority = guiElementPtr->GetPriority();
	QString elementName = guiElementPtr->GetElementName();

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
		QString elementNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), elementName.toUtf8(), languageId, context);

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

	const imtgui::IGuiElementContainer* subElementsPtr = guiElementPtr->GetSubElements();
	if (subElementsPtr != nullptr && m_representationControllerCompPtr.IsValid()){
		imtbase::CTreeItemModel* subElementsModelPtr = representation.AddTreeModel("SubElements");
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


} // namespace imtgui


