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


// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CParamsDataProviderCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CParamsDataProviderCompBase::GetRepresentation(
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

	if (m_translationManagerCompPtr.IsValid()){
		QByteArray languageId;
		if(gqlContext != nullptr){
			languageId = gqlContext->GetLanguageId();
		}

		if (m_paramNameAttrPtr.IsValid()){

			QByteArray context = "Attribute";
			paramName = *m_paramNameAttrPtr;
			QString paramNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, context);
			rootModelPtr->SetData("Name", paramNameTr);
		}
	}
	else{
		rootModelPtr->SetData("Name", paramName);
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
			imtbase::CTreeItemModel* elementModel = m_paramSubElementsCompPtr[i]->GetRepresentation(params, fields, gqlContext);
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


// reimplemented (imtgql::IGqlModelEditor)

bool CParamsDataProviderCompBase::UpdateModelFromRepresentation(
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
				m_mutationDataDelegateCompPtr[i]->UpdateModelFromRepresentation(QList<imtgql::CGqlObject>(), elementsModelPtr);
			}
		}
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
					return false;
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

		rootModelPtr->SetData("Status", "OK");
	}

	return rootModelPtr;
}


} // namespace imtqml


