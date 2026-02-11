// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
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
	virtual bool SetSelectionMode(SelectionMode mode) override;
	virtual Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const Ids& selectedIds) override;

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	/**
		If more than one ID is selected and SM_SINGLE is set, the selection will be reset
	*/
	bool ApplySelectionMode(SelectionMode selectionMode);
	bool ApplySelection(const Ids& selectionIds);

protected:
	virtual void OnConstraintsUpdated(const istd::IChangeable::ChangeSet& changeset, const ICollectionInfo* modelPtr);

private:
	SelectionMode m_selectionMode;
	Ids m_selectedIds;
	imtbase::TModelUpdateBinder<ICollectionInfo, CSelection> m_constraintsObserver;
};


} // namespace imtbase


