#include "CSelectionParamAdapterComp.h"


namespace imtbase
{


// public methods

CSelectionParamAdapterComp::CSelectionParamAdapterComp()
{
	m_constraints.SetParent(this);
}


// protected methods

// reimplemented (ISelection)

const iprm::IOptionsList* CSelectionParamAdapterComp::GetSelectionConstraints() const
{
	return &m_constraints;
}


int CSelectionParamAdapterComp::GetSelectedOptionIndex() const
{
	ICollectionInfo::Ids constraints = m_selectionConstraintsPtr->GetElementIds();
	ISelection::Ids selection = m_selectionCompPtr->GetSelectedIds();

	Q_ASSERT(selection.count() <= 1);

	if (selection.count() > 0){
		int index = constraints.indexOf(*selection.begin());

		return index;
	}

	return -1;
}


bool CSelectionParamAdapterComp::SetSelectedOptionIndex(int index)
{
	ICollectionInfo::Ids constraints = m_selectionConstraintsPtr->GetElementIds();
	ICollectionInfo::Id id;

	if (index >= 0 && index < constraints.count()){
		id = constraints[index];
	}

	if (!id.isEmpty()){
		UpdateBlockerPtr blockerPtr = CreateUpdateBlocker();

		return m_selectionCompPtr->SetSelectedIds({ id });
	}

	return false;
}


iprm::ISelectionParam* CSelectionParamAdapterComp::GetSubselection(int index) const
{
	return nullptr;
}


// reimplemented (CSelectionAdapterBase)

void CSelectionParamAdapterComp::OnConstraintsChanged()
{
	istd::CChangeNotifier notifier1(&m_constraints);

	istd::IChangeable::ChangeSet(CF_CONSTRAINTS_CHANGED);
	istd::CChangeNotifier notifier2(this);
}


void CSelectionParamAdapterComp::OnSelectionChanged()
{
	istd::IChangeable::ChangeSet(CF_SELECTION_CHANGED);
	istd::CChangeNotifier notifier(this);
}


// reimplemented (icomp::CComponentBase)

void CSelectionParamAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	bool retVal = m_selectionCompPtr.IsValid();
	Q_ASSERT(retVal);
	m_selectionPtr = m_selectionCompPtr.GetPtr();

	Q_ASSERT(m_selectionCompPtr->GetSelectionMode() == ISelection::SM_SINGLE);

	imod::IModel* selectionModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(m_selectionCompPtr.GetPtr()));
	Q_ASSERT(selectionModelPtr != nullptr);

	m_selectionConstraintsPtr = m_selectionPtr->GetSelectionConstraints();
	Q_ASSERT(m_selectionConstraintsPtr != nullptr);

	imod::IModel* constraintsModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(m_selectionConstraintsPtr));
	Q_ASSERT(constraintsModelPtr != nullptr);

	retVal = RegisterModel(constraintsModelPtr, MI_CONSTRAINTS);
	Q_ASSERT(retVal);

	retVal = RegisterModel(selectionModelPtr, MI_SELECTION);
	Q_ASSERT(retVal);
}


void CSelectionParamAdapterComp::OnComponentDestroyed()
{
	UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (iser::ISerializable)

bool CSelectionParamAdapterComp::Serialize(iser::IArchive& archive)
{
	return false;
}


// public methopds of the embedded class Constraints

CSelectionParamAdapterComp::Constraints::Constraints()
	:m_parentPtr(nullptr)
{
}


void CSelectionParamAdapterComp::Constraints::SetParent(CSelectionParamAdapterComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (iprm::IOptionsList)

int CSelectionParamAdapterComp::Constraints::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CSelectionParamAdapterComp::Constraints::GetOptionsCount() const
{
	return m_parentPtr->m_selectionConstraintsPtr->GetElementIds().count();
}


QString CSelectionParamAdapterComp::Constraints::GetOptionName(int index) const
{
	ICollectionInfo::Ids constraints = m_parentPtr->m_selectionConstraintsPtr->GetElementIds();

	if (index >= 0 && index < constraints.count()){
		return m_parentPtr->m_selectionConstraintsPtr->GetElementInfo(constraints[index], ICollectionInfo::EIT_NAME).toString();
	}

	return QString();
}


QString CSelectionParamAdapterComp::Constraints::GetOptionDescription(int index) const
{
	ICollectionInfo::Ids constraints = m_parentPtr->m_selectionConstraintsPtr->GetElementIds();

	if (index >= 0 && index < constraints.count()){
		return m_parentPtr->m_selectionConstraintsPtr->GetElementInfo(constraints[index], ICollectionInfo::EIT_DESCRIPTION).toString();
	}

	return QString();
}


QByteArray CSelectionParamAdapterComp::Constraints::GetOptionId(int index) const
{
	ICollectionInfo::Ids constraints = m_parentPtr->m_selectionConstraintsPtr->GetElementIds();

	if (index >= 0 && index < constraints.count()){
		return constraints[index];
	}

	return QByteArray();
}


bool CSelectionParamAdapterComp::Constraints::IsOptionEnabled(int index) const
{
	ICollectionInfo::Ids constraints = m_parentPtr->m_selectionConstraintsPtr->GetElementIds();

	return (index >= 0 && index < constraints.count());
}


} // namespace imtbase


