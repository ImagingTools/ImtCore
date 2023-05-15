#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtbase/ISelection.h>


namespace imtbase
{


/**
	Implementation of the multiple selection.
*/
class CSelection:
			public imod::TSingleModelObserverBase<ICollectionInfo>,
			virtual public ISelection
{
public:
	typedef imod::CSingleModelObserverBase BaseClass;

	CSelection();
	CSelection(SelectionMode selectionMode);

	void SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr);

	// reimplemented (ISelection)
	virtual const ICollectionInfo* GetSelectionConstraints() const override;
	virtual SelectionMode GetSelectionMode() const override;
	virtual bool SetSelectionMode(SelectionMode mode) override;
	virtual Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const Ids& selectedIds) override;

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

private:
	/**
		If more than one ID is selected and SM_SINGLE is set, the selection will be reset
	*/
	bool ApplySelectionMode(SelectionMode selectionMode);
	bool ApplySelection(const Ids& selectionIds);

private:
	SelectionMode m_selectionMode;
	Ids m_selectedIds;
};


} // namespace imtbase


