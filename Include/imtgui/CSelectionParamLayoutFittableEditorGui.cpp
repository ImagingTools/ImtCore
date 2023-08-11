#include <imtgui/CSelectionParamLayoutFittableEditorGui.h>


// Qt includes
#include <QtCore/QStringListModel>

// Acf includes
#include <iprm/IOptionsList.h>


namespace imtgui
{


// public methods

CSelectionParamLayoutFittableEditorGui::CSelectionParamLayoutFittableEditorGui(QObject* parentPtr)
	: QObject(parentPtr)
{
	connect(this, &QObject::objectNameChanged, this, &CSelectionParamLayoutFittableEditorGui::OnObjectNameChanged);
}


// reimplemented (ILayoutFittableWidget)

bool CSelectionParamLayoutFittableEditorGui::PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex, int columnIndex)
{
	int rowToInsert = rowIndex > -1 ? rowIndex : layout.rowCount();

	m_comboBoxPtr = new QComboBox(layout.widget());
	layout.addWidget(m_comboBoxPtr, rowToInsert, columnIndex);

	if (IsModelAttached()){
		ModelType* observedObjectPtr = GetObservedObject();
		QStringListModel* comboBoxModelPtr = new QStringListModel(m_comboBoxPtr);
		QStringList optionNamesList;
		const iprm::IOptionsList* constraintsPtr = observedObjectPtr->GetSelectionConstraints();
		if (constraintsPtr != nullptr){
			int optionsCount = constraintsPtr->GetOptionsCount();
			for (int opionIndex = 0; opionIndex < optionsCount; ++opionIndex){
				optionNamesList << constraintsPtr->GetOptionName(opionIndex);
			}
		}
		comboBoxModelPtr->setStringList(optionNamesList);

		m_comboBoxPtr->setModel(comboBoxModelPtr);
		int currentIndex = observedObjectPtr->GetSelectedOptionIndex();
		m_comboBoxPtr->setCurrentIndex(currentIndex);
	}

	connect(m_comboBoxPtr, &QComboBox::currentTextChanged, this, &CSelectionParamLayoutFittableEditorGui::OnInputValueChanged);

	// trigger method to apply value to child objects
	OnObjectNameChanged(objectName());

	return true;
}


// reimplemented (imod::IObserver)

bool CSelectionParamLayoutFittableEditorGui::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool isModelAttached = BaseClass::OnModelAttached(modelPtr, changeMask);

	if (isModelAttached && !m_comboBoxPtr.isNull()){
		QSignalBlocker comboBlock(m_comboBoxPtr);
		Q_UNUSED(comboBlock);

		ModelType* observedObjectPtr = GetObservedObject();
		QStringListModel* comboBoxModelPtr = new QStringListModel(m_comboBoxPtr);
		QStringList optionNamesList;
		const iprm::IOptionsList* constraintsPtr = observedObjectPtr->GetSelectionConstraints();
		if (constraintsPtr != nullptr){
			int optionsCount = constraintsPtr->GetOptionsCount();
			for (int opionIndex = 0; opionIndex < optionsCount; ++opionIndex){
				optionNamesList << constraintsPtr->GetOptionName(opionIndex);
			}
		}
		comboBoxModelPtr->setStringList(optionNamesList);

		m_comboBoxPtr->setModel(comboBoxModelPtr);
		int currentIndex = observedObjectPtr->GetSelectedOptionIndex();
		m_comboBoxPtr->setCurrentIndex(currentIndex);
	}

	return isModelAttached;
}


// reimplemented (imod::IModelEditor)

void CSelectionParamLayoutFittableEditorGui::UpdateEditor(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_comboBoxPtr.isNull()){
		ModelType* observedObjectPtr = GetObservedObject();
		int currentIndex = observedObjectPtr->GetSelectedOptionIndex();
		m_comboBoxPtr->setCurrentIndex(currentIndex);
	}
}


void CSelectionParamLayoutFittableEditorGui::UpdateModelFromEditor() const
{
	if (!m_comboBoxPtr.isNull()){
		ModelType* observedObjectPtr = GetObservedObject();
		observedObjectPtr->SetSelectedOptionIndex(m_comboBoxPtr->currentIndex());
	}
}


bool CSelectionParamLayoutFittableEditorGui::IsReadOnly() const
{
	return !m_comboBoxPtr.isNull() && m_comboBoxPtr->isEnabled();
}


void CSelectionParamLayoutFittableEditorGui::SetReadOnly(bool state)
{
	if (!m_comboBoxPtr.isNull() && m_comboBoxPtr->isEnabled() != state){
		m_comboBoxPtr->setEnabled(state);
	}
}


void CSelectionParamLayoutFittableEditorGui::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateEditor(changeSet);
}


void CSelectionParamLayoutFittableEditorGui::OnObjectNameChanged(const QString newObjectName)
{
	if (!m_comboBoxPtr.isNull()){
		m_comboBoxPtr->setObjectName(newObjectName + "LineEdit");
	}
}


void CSelectionParamLayoutFittableEditorGui::OnInputValueChanged()
{
	ModelType* observedObjectPtr = GetObservedObject();
	observedObjectPtr->SetSelectedOptionIndex(m_comboBoxPtr->currentIndex());
}



} // namespace imtgui
