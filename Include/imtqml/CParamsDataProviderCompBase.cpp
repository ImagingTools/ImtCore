#include <imtqml/CParamsDataProviderCompBase.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ITextParam.h>


namespace imtqml
{


// public methods

const imtbase::CTreeItemModel* CParamsDataProviderCompBase::GetElementModel(const QByteArray &elementId, const imtbase::CTreeItemModel* elementsModelPtr) const
{
	for (int i = 0; i < elementsModelPtr->GetItemsCount(); i++){
		if (elementsModelPtr->ContainsKey("Id", i)){
			QByteArray currentElementId = elementsModelPtr->GetData("Id", i).toByteArray();
			if (currentElementId == elementId){
				return elementsModelPtr->GetModelFromItem(i);
			}
		}
	}

	return nullptr;
}

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CParamsDataProviderCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CParamsDataProviderCompBase::GetTreeItemModel(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId;
	QString paramName;

	if (m_paramIdAttrPtr.IsValid()){
		paramId = *m_paramIdAttrPtr;
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	const QTranslator* translatorPtr = nullptr;
	int currentIndex = -1;
	if (m_translationManagerCompPtr.IsValid()){
		QByteArray languageId = GetLanguageIdFromInputParams(params);

		if (languageId.isEmpty()){
//			currentIndex = 2;
//			languageId = "ru_RU";

			currentIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
		}
		else{
			currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
		}

		if (currentIndex >= 0){
			translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
		}
	}

	if (m_paramNameAttrPtr.IsValid()){
		paramName = *m_paramNameAttrPtr;
		if (translatorPtr != nullptr){
			if (m_translationManagerCompPtr->GetSlaveTranslationManager() != nullptr && currentIndex >= 0){
				const QTranslator* slaveTranslatorPtr = m_translationManagerCompPtr->GetSlaveTranslationManager()->GetLanguageTranslator(currentIndex);
				QString text1 = slaveTranslatorPtr->translate("Attribute", paramName.toUtf8());
//				rootModelPtr->SetData("Name", slaveTranslatorPtr->translate("Attribute", paramName.toUtf8()));
				rootModelPtr->SetData("Name", paramName);
			}
		}
		else{
			rootModelPtr->SetData("Name", paramName);
		}
	}

	int type = 0;;
	if (m_paramComponentTypeAttrPtr.IsValid()){
		type = *m_paramComponentTypeAttrPtr;
	}

	QString componentType;
	switch(type){
		case CT_UNKNOWN:
			componentType = "UNKNOWN";
		break;
		case CT_TEXT_INPUT:
			componentType = "TextInput";
		break;
		case CT_INTEGER_INPUT:
			componentType = "IntegerInput";
		break;
		case CT_COMBOBOX:
			componentType = "ComboBox";
		break;
		case CT_BUTTON:
			componentType = "Button";
		break;
		case CT_TEXT_LABEL:
			componentType = "TextLabel";
		break;
	}

	rootModelPtr->SetData("ComponentType", componentType);

	if (m_paramSubElementsCompPtr.IsValid()){
		imtbase::CTreeItemModel* externModel = new imtbase::CTreeItemModel();

		for (int i = 0; i < m_paramSubElementsCompPtr.GetCount(); i++){
			imtbase::CTreeItemModel* elementModel = m_paramSubElementsCompPtr[i]->GetTreeItemModel(params, fields);
			int externIndex = externModel->InsertNewItem();
			externModel->CopyItemDataFromModel(externIndex, elementModel);
		}

		rootModelPtr->SetExternTreeModel("Elements", externModel);
	}
	else{
		if (type == CT_COMBOBOX){
			iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
			if (selectionParam != nullptr){
				const iprm::IOptionsList* optionList = selectionParam->GetSelectionConstraints();
				int defaultIndex = selectionParam->GetSelectedOptionIndex();
				rootModelPtr->SetData("Value", defaultIndex);
				imtbase::CTreeItemModel* parametersPtr = rootModelPtr->AddTreeModel("Parameters");
				parametersPtr->SetIsArray(true);
				for (int i = 0; i < optionList->GetOptionsCount(); i++){
					QByteArray id = optionList->GetOptionId(i);
					QString name = optionList->GetOptionName(i);

					int index = parametersPtr->InsertNewItem();

					parametersPtr->SetData("Id", id, index);
					parametersPtr->SetData("Name", name, index);
				}
			}
		}
		else if (type == CT_TEXT_INPUT){
			iprm::ITextParam* sourcePtr = dynamic_cast<iprm::ITextParam*>(m_parameterCompPtr.GetPtr());
			if (sourcePtr != nullptr){
				QString value = sourcePtr->GetText();
				rootModelPtr->SetData("Value", value);
			}
		}
		else if (type == CT_BUTTON){
			iprm::ITextParam* sourcePtr = dynamic_cast<iprm::ITextParam*>(m_parameterCompPtr.GetPtr());
			if (sourcePtr != nullptr){
				QString value = sourcePtr->GetText();
				rootModelPtr->SetData("Value", value);
			}
		}
		else if (type == CT_TEXT_LABEL){
			iprm::ITextParam* sourcePtr = dynamic_cast<iprm::ITextParam*>(m_parameterCompPtr.GetPtr());
			if (sourcePtr != nullptr){
				QString value = sourcePtr->GetText();
				rootModelPtr->SetData("Value", value);
			}
		}
	}

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

imtbase::CTreeItemModel* CParamsDataProviderCompBase::UpdateBaseModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModel,
		const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray parameterId = *m_paramIdAttrPtr;

	rootModelPtr->SetData("Id", parameterId);
	rootModelPtr->SetData("Name", *m_paramNameAttrPtr);

	if (!m_parameterCompPtr.IsValid()){
		const imtbase::CTreeItemModel* pageModelPtr = GetElementModel(parameterId, baseModel);
		if (pageModelPtr != nullptr){
			imtbase::CTreeItemModel* elementsModelPtr = pageModelPtr->GetTreeItemModel("Elements");

			for (int i = 0; i < m_mutationDataDelegateCompPtr.GetCount(); i++){
				m_mutationDataDelegateCompPtr[i]->UpdateBaseModelFromRepresentation(QList<imtgql::CGqlObject>(), elementsModelPtr);
			}
		}

//		if (baseModel->ContainsKey("Elements")){
//			imtbase::CTreeItemModel* elementsModelPtr = baseModel->GetTreeItemModel("Elements");

//			QList<imtgql::CGqlObject> parametersList;
//			for (int i = 0; i < m_mutationDataDelegateCompPtr.GetCount(); i++){
//				m_mutationDataDelegateCompPtr[i]->UpdateBaseModelFromRepresentation(parametersList, elementsModelPtr);
//			}

//	//		imtbase::CTreeItemModel* rootElementsModel = rootModelPtr->AddTreeModel("Elements");

//			for (int i = 0; i < m_mutationDataDelegateCompPtr.GetCount(); i++){
//				imtbase::CTreeItemModel elementModel;
//				elementsModelPtr->CopyItemDataToModel(i, &elementModel);
//				QList<imtgql::CGqlObject> parametersList;

//				if (elementModel.ContainsKey("Id")){
//					QByteArray elementId = elementModel.GetData("Id").toByteArray();
//					imtgql::CGqlObject gqlObject;
//					gqlObject.InsertField("Id", QString(elementId));
//					parametersList << gqlObject;
//				}

//				imtbase::CTreeItemModel* externModel = m_mutationDataDelegateCompPtr[i]->UpdateBaseModelFromRepresentation(parametersList, &elementModel);
//	//			if (externModel != nullptr){
//	//				for (int j = 0; j < externModel->GetItemsCount(); j++){
//	//					int index = rootElementsModel->InsertNewItem();
//	//					rootElementsModel->CopyItemDataFromModel(index, externModel, j);
//	//				}
//	//			}
//			}
//		}
	}
	else {
		const imtbase::CTreeItemModel* elementModelPtr = GetElementModel(parameterId, baseModel);
		if (elementModelPtr != nullptr){
			int type = 0;
			if (m_paramComponentTypeAttrPtr.IsValid()){
				type = *m_paramComponentTypeAttrPtr;
			}

			if (type == CT_COMBOBOX){
				iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
				if (selectionParam == nullptr){
					return nullptr;
				}

				int value = 0;
				if (elementModelPtr->ContainsKey("Value")){
					value = elementModelPtr->GetData("Value").toInt();
				}

				selectionParam->SetSelectedOptionIndex(value);
			}
			else if (type == CT_TEXT_INPUT){
				iprm::ITextParam* sourcePtr = dynamic_cast<iprm::ITextParam*>(m_parameterCompPtr.GetPtr());

				QString value;
				if (elementModelPtr->ContainsKey("Value")){
					value = elementModelPtr->GetData("Value").toString();
				}

				if (sourcePtr != nullptr){
					sourcePtr->SetText(value);
				}
			}
		}

//		int type = 0;
//		if (m_paramComponentTypeAttrPtr.IsValid()){
//			type = *m_paramComponentTypeAttrPtr;
//		}

//		if (type == CT_COMBOBOX){
//			iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
//			if (selectionParam == nullptr){
//				return nullptr;
//			}

//			int value = 0;
//			if (baseModel->ContainsKey("Value")){
//				value = baseModel->GetData("Value").toInt();
//			}

//			selectionParam->SetSelectedOptionIndex(value);
//		}
//		else if (type == CT_TEXT_INPUT){
//			iprm::ITextParam* sourcePtr = dynamic_cast<iprm::ITextParam*>(m_parameterCompPtr.GetPtr());

//			QString value;
//			if (baseModel->ContainsKey("Value")){
//				value = baseModel->GetData("Value").toString();
//			}

//			if (sourcePtr != nullptr){
//				sourcePtr->SetText(value);
//			}
//		}
		rootModelPtr->SetData("Status", "OK");
	}

	return rootModelPtr;
}


} // namespace imtqml


