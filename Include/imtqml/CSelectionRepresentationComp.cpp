#include <imtqml/CSelectionRepresentationComp.h>


// ACF includes
#include <iprm/IOptionsList.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CSelectionRepresentationComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId = *m_paramIdAttrPtr;
	QString paramName = *m_paramNameAttrPtr;

	rootModelPtr->SetData("Id", paramId);
	rootModelPtr->SetData("Name", paramName);

	if (m_qmlPathAttrPtr.IsValid()){
		rootModelPtr->SetData("Source", *m_qmlPathAttrPtr);
	}

	const iprm::IOptionsList* optionListPtr = m_selectionParamCompPtr->GetSelectionConstraints();

	if (optionListPtr != nullptr){
		int defaultIndex = m_selectionParamCompPtr->GetSelectedOptionIndex();

		rootModelPtr->SetData("Value", defaultIndex);

		imtbase::CTreeItemModel* parametersPtr = rootModelPtr->AddTreeModel("Parameters");

		Q_ASSERT(parametersPtr != nullptr);

		for (int i = 0; i < optionListPtr->GetOptionsCount(); i++){
			QByteArray id = optionListPtr->GetOptionId(i);
			QString name = optionListPtr->GetOptionName(i);

			int index = parametersPtr->InsertNewItem();

			parametersPtr->SetData("Id", id, index);
			parametersPtr->SetData("Name", name, index);
		}
	}

	return rootModelPtr;
}

bool CSelectionRepresentationComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_selectionParamCompPtr.IsValid()){
		return false;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray parameterId = *m_paramIdAttrPtr;

	rootModelPtr->SetData("Id", parameterId);
	rootModelPtr->SetData("Name", *m_paramNameAttrPtr);

	if (representationPtr != nullptr){
		int value = 0;
		if (representationPtr->ContainsKey("Value")){
			value = representationPtr->GetData("Value").toInt();
		}

		m_selectionParamCompPtr->SetSelectedOptionIndex(value);
	}

	return true;
}


} // namespace imtqml


