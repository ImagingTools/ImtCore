#include <imtqml/CParamsDataProviderCompBase.h>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>


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

	if (m_parameterCompPtr.IsValid()){
		iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());

		if (selectionParam != nullptr){

			if (params.size() > 0){
				QByteArray itemData = params.at(0).GetFieldArgumentValue("Item").toByteArray();

				if (!itemData.isEmpty()){
					imtbase::CTreeItemModel itemModel;
					itemModel.Parse(itemData);

					if (itemModel.ContainsKey("items")){

						QByteArray itemsData = itemModel.GetData("items").toByteArray();

						imtbase::CTreeItemModel* itemsModel = itemModel.GetTreeItemModel("items");

						if (itemsModel != nullptr){
							for (int i = 0; i < itemsModel->GetItemsCount(); i++){
								QByteArray itemId = itemsModel->GetData("Id", i).toByteArray();

								if (itemId == "General"){
									imtbase::CTreeItemModel* elements  = itemsModel->GetTreeItemModel("Elements", i);

									if (elements != nullptr){

										for (int j = 0; j < elements->GetItemsCount(); j++){
											QByteArray id = elements->GetData("Id", j).toByteArray();
											int value = elements->GetData("Value", j).toInt();

											if (id == paramId){
												selectionParam->SetSelectedOptionIndex(value);
												break;
											}
										}
									}
									break;
								}
							}
						}
					}
				}
			}

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

	if (m_paramSubElementCompPtr.IsValid()){
		imtbase::CTreeItemModel* externModel = m_paramSubElementCompPtr->GetTreeItemModel(params, fields);
		rootModelPtr->SetExternTreeModel("Elements", externModel);
	}

	return rootModelPtr;
}


} // namespace imtqml


