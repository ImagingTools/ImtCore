#include <imtbase/CSingleSelection.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


// reimplemented (ISingleSelection)

const ICollectionInfo* CSingleSelection::GetSelectionConstraints() const
{
	return GetObservedObject();
}


ISingleSelection::Id CSingleSelection::GetSelectedId() const
{
	return m_selectedId;
}


bool CSingleSelection::SetSelectedId(const Id& selectedId)
{
	if (m_selectedId != selectedId){
		ICollectionInfo* constraintsPtr = GetObservedObject();
		if (constraintsPtr != nullptr){
			if (!constraintsPtr->GetElementIds().contains(selectedId)){
				return false;
			}
		}

		PrivateSetSelectedId(selectedId);
	}

	return true;
}


// reimplement (iser::ISerializable)

bool CSingleSelection::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	QByteArray selectedId = m_selectedId;

	static iser::CArchiveTag selectedItemTag("SelectedItem", "ID of the selected item");
	retVal = retVal && archive.BeginTag(selectedItemTag);
	retVal = retVal && archive.Process(selectedId);
	retVal = retVal && archive.EndTag(selectedItemTag);

	if (!archive.IsStoring()){
		if (!retVal){
			selectedId.clear();
		}

		SetSelectedId(selectedId);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CSingleSelection::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CSingleSelection::CopyFrom(const IChangeable & object, CompatibilityMode /*mode*/)
{
	const CSingleSelection* sourcePtr = dynamic_cast<const CSingleSelection*>(&object);
	if (sourcePtr != nullptr){
		if (m_selectedId != sourcePtr->m_selectedId){
			return SetSelectedId(sourcePtr->m_selectedId);
		}

		return true;
	}

	return false;
}


bool CSingleSelection::IsEqual(const IChangeable & object) const
{
	const CSingleSelection* sourcePtr = dynamic_cast<const CSingleSelection*>(&object);
	if (sourcePtr != nullptr){
		return (m_selectedId == sourcePtr->m_selectedId);
	}

	return false;
}


istd::IChangeable* CSingleSelection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CSingleSelection> clonePtr(new CSingleSelection());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CSingleSelection::ResetData(CompatibilityMode /*mode*/)
{
	if (!m_selectedId.isEmpty()){
		PrivateSetSelectedId(QByteArray());
	}

	return true;
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CSingleSelection::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!GetObservedObject()->GetElementIds().contains(m_selectedId)){
		PrivateSetSelectedId(QByteArray());
	}
}


// private methods

void CSingleSelection::PrivateSetSelectedId(const Id& selectedId)
{
	SelectionChangeInfo info;
	info.selectedId = selectedId;
	istd::IChangeable::ChangeSet changeSet;
	changeSet.SetChangeInfo(s_cidSelectionChanged, QVariant::fromValue<SelectionChangeInfo>(info));
	istd::CChangeNotifier notifier(this, &changeSet);

	m_selectedId = selectedId;
}


} // namespace imtbase


