#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/ISingleSelection.h>


namespace imtbase
{


/**
	Implementation of the single selection.
*/
class CSingleSelection:
			public imod::TSingleModelObserverBase<ICollectionInfo>,
			virtual public ISingleSelection
{
public:
	typedef imod::TSingleModelObserverBase<ICollectionInfo> BaseClass;

	// reimplemented (ISingleSelection)
	virtual const ICollectionInfo* GetSelectionConstraints() const override;
	virtual Id GetSelectedId() const override;
	virtual bool SetSelectedId(const Id& selectedId) override;

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
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void PrivateSetSelectedId(const Id& selectedId);

private:
	QByteArray m_selectedId;
};


} // namespace imtbase


