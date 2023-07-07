#include "CSelectionAdapterComp.h"


namespace imtbase
{


// public methods

CSelectionAdapterComp::CSelectionAdapterComp()
{
	m_constraints.SetParent(this);
}


// protected methods

// reimplemented (ISelection)

const ICollectionInfo* CSelectionAdapterComp::GetSelectionConstraints() const
{
	return &m_constraints;
}


ISelection::SelectionMode CSelectionAdapterComp::GetSelectionMode() const
{
	return SM_SINGLE;
}


bool CSelectionAdapterComp::SetSelectionMode(SelectionMode /*mode*/)
{
	return false;
}


ISelection::Ids CSelectionAdapterComp::GetSelectedIds() const
{
	int count = m_selectionConstraintsPtr->GetOptionsCount();
	int index = m_selectionCompPtr->GetSelectedOptionIndex();
	if (index >= 0 && index < count){
		return Ids({ m_selectionConstraintsPtr->GetOptionId(index) });
	}

	return Ids();
}


bool CSelectionAdapterComp::SetSelectedIds(const Ids& selectedIds)
{
	if (selectedIds.count() <= 1){
		if (selectedIds.isEmpty()){
			istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
			istd::CChangeNotifier notifier(this, &changeSet);

			UpdateBlockerPtr blockerPtr = CreateUpdateBlocker();

			return m_selectionCompPtr->SetSelectedOptionIndex(-1);
		}
		else{
			int index = GetOptionIndex(*selectedIds.begin());

			if (index >= 0){
				istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
				istd::CChangeNotifier notifier(this, &changeSet);

				UpdateBlockerPtr blockerPtr = CreateUpdateBlocker();

				return m_selectionCompPtr->SetSelectedOptionIndex(index);
			}
		}
	}

	return false;
}


// reimplemented (CSelectionAdapterBase)

void CSelectionAdapterComp::OnConstraintsChanged()
{
	istd::CChangeNotifier notifier1(&m_constraints);

	istd::IChangeable::ChangeSet(CF_CONSTRAINTS_CHANGED);
	istd::CChangeNotifier notifier2(this);
}


void CSelectionAdapterComp::OnSelectionChanged()
{
	istd::IChangeable::ChangeSet(CF_SELECTION_CHANGED);
	istd::CChangeNotifier notifier(this);
}


// reimplemented (icomp::CComponentBase)

void CSelectionAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	bool retVal = m_selectionCompPtr.IsValid();
	Q_ASSERT(retVal);
	m_selectionPtr = m_selectionCompPtr.GetPtr();

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


void CSelectionAdapterComp::OnComponentDestroyed()
{
	UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (iser::ISerializable)

bool CSelectionAdapterComp::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// private methods

int CSelectionAdapterComp::GetOptionIndex(const QByteArray& optionId) const
{
	int count = m_selectionConstraintsPtr->GetOptionsCount();
	for (int i = 0; i < count; i++){
		if (m_selectionConstraintsPtr->GetOptionId(i) == optionId){
			return i;
		}
	}

	return -1;
}


QByteArray CSelectionAdapterComp::GetOptionId(int index) const
{
	int count = m_selectionConstraintsPtr->GetOptionsCount();
	if (index >= 0 && index < count){
		return m_selectionConstraintsPtr->GetOptionId(index);
	}

	return QByteArray();
}


// public methopds of the embedded class Constraints

CSelectionAdapterComp::Constraints::Constraints()
	:m_parentPtr(nullptr)
{
}


void CSelectionAdapterComp::Constraints::SetParent(CSelectionAdapterComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (ICollectionInfo)

int CSelectionAdapterComp::Constraints::GetElementsCount(const iprm::IParamsSet* selectionParamPtr) const
{
	Q_ASSERT(selectionParamPtr == nullptr);

	return m_parentPtr->m_selectionConstraintsPtr->GetOptionsCount();
}


ICollectionInfo::Ids CSelectionAdapterComp::Constraints::GetElementIds(int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const
{
	Q_ASSERT(offset == 0);
	Q_ASSERT(count == -1);
	Q_ASSERT(selectionParamsPtr == nullptr);

	ICollectionInfo::Ids retVal;

	int optionCount = m_parentPtr->m_selectionConstraintsPtr->GetOptionsCount();
	for (int i = 0; i < optionCount; i++){
		retVal += m_parentPtr->m_selectionConstraintsPtr->GetOptionId(i);
	}

	return retVal;
}


bool CSelectionAdapterComp::Constraints::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


QVariant CSelectionAdapterComp::Constraints::GetElementInfo(const Id& elementId, int infoType) const
{
	int index = m_parentPtr->GetOptionIndex(elementId);
	Q_ASSERT(index >= 0);

	if (index >= 0){
		ICollectionInfo::Ids ids = GetElementIds();

		if (ids.contains(elementId)){
			switch (infoType){
			case ICollectionInfo::EIT_NAME:
			{
				QString val = m_parentPtr->m_selectionConstraintsPtr->GetOptionName(index);
				return QVariant(val);
			}
			case ICollectionInfo::EIT_DESCRIPTION:
			{
				QString val = m_parentPtr->m_selectionConstraintsPtr->GetOptionDescription(index);
				return QVariant(val);
			}
			case ICollectionInfo::EIT_ENABLED:
			{
				bool val = m_parentPtr->m_selectionConstraintsPtr->IsOptionEnabled(index);
				return QVariant(val);
			}
			default:
				Q_ASSERT(false);
			}
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CSelectionAdapterComp::Constraints::GetElementMetaInfo(const Id& /*elementId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CSelectionAdapterComp::Constraints::SetElementName(const Id& /*elementId*/, const QString& /*name*/)
{
	return false;
}


bool CSelectionAdapterComp::Constraints::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/)
{
	return false;
}


bool CSelectionAdapterComp::Constraints::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/)
{
	return false;
}


} // namespace imtbase


