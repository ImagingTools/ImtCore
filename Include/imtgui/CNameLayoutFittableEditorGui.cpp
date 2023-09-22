#include <imtgui/CNameLayoutFittableEditorGui.h>



namespace imtgui
{


// public methods

CNameLayoutFittableEditorGui::CNameLayoutFittableEditorGui(QObject* parentPtr)
	: QObject(parentPtr)
{
	connect(this, &QObject::objectNameChanged, this, &CNameLayoutFittableEditorGui::OnObjectNameChanged);
}


// reimplemented (ILayoutFittableWidget)

bool CNameLayoutFittableEditorGui::PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex, int columnIndex)
{
	int rowToInsert = rowIndex > -1 ? rowIndex : layout.rowCount();

	m_nameLineEditPtr = new QLineEdit(layout.widget());
	layout.addWidget(m_nameLineEditPtr, rowToInsert, columnIndex);

	if (IsModelAttached()){
		ModelType* observedObjectPtr = GetObservedObject();
		m_nameLineEditPtr->setText(observedObjectPtr->GetName());
	}

	connect(m_nameLineEditPtr, &QLineEdit::textChanged, this, &CNameLayoutFittableEditorGui::OnInputValueChanged);

	// trigger method to apply value to child objects
	OnObjectNameChanged(objectName());

	return true;
}


// reimplemented (imod::IObserver)

bool CNameLayoutFittableEditorGui::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool isModelAttached = BaseClass::OnModelAttached(modelPtr, changeMask);

	if (isModelAttached && !m_nameLineEditPtr.isNull()){
		QSignalBlocker nameEditorBlock(m_nameLineEditPtr);
		Q_UNUSED(nameEditorBlock);

		ModelType* observedObjectPtr = GetObservedObject();
		m_nameLineEditPtr->setText(observedObjectPtr->GetName());
	}

	return isModelAttached;
}


// reimplemented (imod::IModelEditor)

void CNameLayoutFittableEditorGui::UpdateEditor(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	ModelType* observedObjectPtr = GetObservedObject();
	if (!m_nameLineEditPtr.isNull()){
		m_nameLineEditPtr->setText(observedObjectPtr->GetName());
	}
}


void CNameLayoutFittableEditorGui::UpdateModelFromEditor() const
{
	if (!m_nameLineEditPtr.isNull()){
		ModelType* observedObjectPtr = GetObservedObject();
		observedObjectPtr->SetName(m_nameLineEditPtr->text());
	}
}


bool CNameLayoutFittableEditorGui::IsReadOnly() const
{
	return !m_nameLineEditPtr.isNull() && m_nameLineEditPtr->isEnabled();
}


void CNameLayoutFittableEditorGui::SetReadOnly(bool state)
{
	if (!m_nameLineEditPtr.isNull() && m_nameLineEditPtr->isEnabled() != state){
		m_nameLineEditPtr->setEnabled(!state);
	}
}


void CNameLayoutFittableEditorGui::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateEditor(changeSet);
}


void CNameLayoutFittableEditorGui::OnObjectNameChanged(const QString newObjectName)
{
	if (!m_nameLineEditPtr.isNull()){
		m_nameLineEditPtr->setObjectName(newObjectName + "LineEdit");
	}
}


void CNameLayoutFittableEditorGui::OnInputValueChanged()
{
	ModelType* observedObjectPtr = GetObservedObject();
	observedObjectPtr->SetName(m_nameLineEditPtr->text());
}



} // namespace imtgui
