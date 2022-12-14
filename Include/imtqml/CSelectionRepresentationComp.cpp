#include <imtqml/CSelectionRepresentationComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

imtbase::CTreeItemModel* CSelectionRepresentationComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	if (!m_selectionParamCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId = *m_paramIdAttrPtr;
	QString paramName = *m_paramNameAttrPtr;\
	QString sourceQml = *m_qmlPathAttrPtr;

	rootModelPtr->SetData("Id", paramId);
	rootModelPtr->SetData("Name", paramName);
	rootModelPtr->SetData("Source", sourceQml);

	const iprm::IOptionsList* optionListPtr = m_selectionParamCompPtr->GetSelectionConstraints();

	Q_ASSERT(optionListPtr != nullptr);

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

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlModelEditor)

bool CSelectionRepresentationComp::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModel,
		const imtgql::IGqlContext* gqlContext)
{
	if (!m_selectionParamCompPtr.IsValid()){
		return false;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray parameterId = *m_paramIdAttrPtr;

	rootModelPtr->SetData("Id", parameterId);
	rootModelPtr->SetData("Name", *m_paramNameAttrPtr);

	const imtbase::CTreeItemModel* elementModelPtr = GetElementModel(parameterId, baseModel);
	if (elementModelPtr != nullptr){
		int value = 0;
		if (elementModelPtr->ContainsKey("Value")){
			value = elementModelPtr->GetData("Value").toInt();
		}

		m_selectionParamCompPtr->SetSelectedOptionIndex(value);
	}

	return true;
}


} // namespace imtqml


