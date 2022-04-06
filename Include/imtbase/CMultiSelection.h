#pragma once


// ImtCore includes
#include <imtbase/IMultiSelection.h>


namespace imtbase
{


/**
	Implementation of the multiple selection.
*/
class CMultiSelection: virtual public IMultiSelection
{
public:
	// reimplemented (IMultiSelection)
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
	Ids m_selectedIds;
};


} // namespace imtbase


