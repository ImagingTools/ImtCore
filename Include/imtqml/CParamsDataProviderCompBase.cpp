#include <imtqml/CParamsDataProviderCompBase.h>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ITextParam.h>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CParamsDataProviderCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CParamsDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,
																	   const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId;
	QString paramName;

	if (m_paramIdAttrPtr.IsValid()){
		paramId = *m_paramIdAttrPtr;
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	if (m_paramNameAttrPtr.IsValid()){
		paramName = *m_paramNameAttrPtr;
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
	}

	rootModelPtr->SetData("ComponentType", componentType);

	if (m_paramSubElementsCompPtr.IsValid()){
		imtbase::CTreeItemModel* externModel = new imtbase::CTreeItemModel();
		externModel->SetIsArray(true);

		for (int i = 0; i < m_paramSubElementsCompPtr.GetCount(); i++){
			imtbase::CTreeItemModel* elementModel = m_paramSubElementsCompPtr[i]->GetTreeItemModel(params, fields);
			externModel->InsertNewItem();
			externModel->CopyItemDataFromModel(i, elementModel);
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
	}

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

imtbase::CTreeItemModel* CParamsDataProviderCompBase::UpdateBaseModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModel)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (m_paramIdAttrPtr.IsValid()){
		rootModel->SetData("Id", *m_paramIdAttrPtr);
	}

	if (m_paramNameAttrPtr.IsValid()){
		rootModel->SetData("Name", *m_paramNameAttrPtr);
	}

	if (!m_parameterCompPtr.IsValid()){

		if (m_mutationDataDelegateCompPtr.IsValid()){
			imtbase::CTreeItemModel* elementsModel = baseModel->GetTreeItemModel("Elements");

			imtbase::CTreeItemModel* rootElementsModel = rootModel->AddTreeModel("Elements");

			for (int index = 0; index < m_mutationDataDelegateCompPtr.GetCount(); index++){
				imtbase::CTreeItemModel* elementModel = new imtbase::CTreeItemModel();
				elementsModel->CopyItemDataToModel(index, elementModel);

				imtbase::CTreeItemModel* externModel = m_mutationDataDelegateCompPtr[index]->UpdateBaseModelFromRepresentation(params, elementModel);
				rootElementsModel->InsertNewItem();
				rootElementsModel->CopyItemDataFromModel(index, externModel);
			}
		}
	}
	else{
		iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());

		if (selectionParam == nullptr){
			return nullptr;
		}

		int value = 0;

		if (baseModel->ContainsKey("Value")){
			value = baseModel->GetData("Value").toInt();
		}

		selectionParam->SetSelectedOptionIndex(value);

		rootModel->SetData("Status", "OK");
	}

	return rootModel;
}


} // namespace imtqml


