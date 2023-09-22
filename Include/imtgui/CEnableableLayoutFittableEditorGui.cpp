#include <imtgui/CEnableableLayoutFittableEditorGui.h>



namespace imtgui
{


// public methods

CEnableableLayoutFittableEditorGui::CEnableableLayoutFittableEditorGui(QObject* parentPtr)
	: QObject(parentPtr)
{
	connect(this, &QObject::objectNameChanged, this, &CEnableableLayoutFittableEditorGui::OnObjectNameChanged);
}


// reimplemented (ILayoutFittableWidget)

bool CEnableableLayoutFittableEditorGui::PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex, int columnIndex)
{
	int rowToInsert = rowIndex > -1 ? rowIndex : layout.rowCount();

	m_checkBoxPtr = new QCheckBox(layout.widget());
	layout.addWidget(m_checkBoxPtr, rowToInsert, columnIndex);

	if (IsModelAttached()){
		ModelType* observedObjectPtr = GetObservedObject();
		m_checkBoxPtr->setChecked(observedObjectPtr->IsEnabled());
	}

	connect(m_checkBoxPtr, &QCheckBox::clicked, this, &CEnableableLayoutFittableEditorGui::OnCheckBoxClicked);

	// trigger method to apply value to child objects
	OnObjectNameChanged(objectName());

	return true;
}


// reimplemented (imod::IObserver)

bool CEnableableLayoutFittableEditorGui::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool isModelAttached = BaseClass::OnModelAttached(modelPtr, changeMask);

	if (isModelAttached && !m_checkBoxPtr.isNull()){
		QSignalBlocker nameEditorBlock(m_checkBoxPtr);
		Q_UNUSED(nameEditorBlock);

		ModelType* observedObjectPtr = GetObservedObject();
		m_checkBoxPtr->setChecked(observedObjectPtr->IsEnabled());
	}

	return isModelAttached;
}


// reimplemented (imod::IModelEditor)

void CEnableableLayoutFittableEditorGui::UpdateEditor(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	ModelType* observedObjectPtr = GetObservedObject();
	if (!m_checkBoxPtr.isNull()){
		m_checkBoxPtr->setChecked(observedObjectPtr->IsEnabled());
	}
}


void CEnableableLayoutFittableEditorGui::UpdateModelFromEditor() const
{
	if (!m_checkBoxPtr.isNull()){
		ModelType* observedObjectPtr = GetObservedObject();
		observedObjectPtr->SetEnabled(m_checkBoxPtr->isChecked());
	}
}


bool CEnableableLayoutFittableEditorGui::IsReadOnly() const
{
	return !m_checkBoxPtr.isNull() && m_checkBoxPtr->isEnabled();
}


void CEnableableLayoutFittableEditorGui::SetReadOnly(bool state)
{
	if (!m_checkBoxPtr.isNull() && m_checkBoxPtr->isEnabled() != state){
		m_checkBoxPtr->setEnabled(!state);
	}
}


void CEnableableLayoutFittableEditorGui::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateEditor(changeSet);
}


void CEnableableLayoutFittableEditorGui::OnObjectNameChanged(const QString newObjectName)
{
	if (!m_checkBoxPtr.isNull()){
		m_checkBoxPtr->setObjectName(newObjectName + "CheckBox");
	}
}


void CEnableableLayoutFittableEditorGui::OnCheckBoxClicked()
{
	ModelType* observedObjectPtr = GetObservedObject();
	observedObjectPtr->SetEnabled(m_checkBoxPtr->isChecked());
}



} // namespace imtgui
