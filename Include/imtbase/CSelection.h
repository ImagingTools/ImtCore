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
class CSelection: virtual public ISelection
{
public:
	CSelection();
	CSelection(SelectionMode selectionMode);

	void SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr);

	// reimplemented (ISelection)
	virtual const ICollectionInfo* GetSelectionConstraints() const override;
	virtual SelectionMode GetSelectionMode() const override;
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

private:
	class ConstraintsObserver: public imod::TSingleModelObserverBase<ICollectionInfo>
	{
	public:
		ConstraintsObserver(CSelection& parent);

	protected:
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeset) override;

	private:
		CSelection& m_parent;
	};

private:
	/**
		Returns only identifiers contained (or not contained) in constraints
	*/
	Ids FilterIds(Ids ids, bool containedInConstraints) const;

	/**
		Apply identifiers to selection and emit notification
	*/
	void ApplyNewIds(Ids ids);

private:
	SelectionMode m_selectionMode;
	Ids m_selectedIds;
	ConstraintsObserver m_constraintsObserver;

	mutable QSet<Id> m_constraintsCache;
};


} // namespace imtbase


