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
	virtual const iprm::IOptionsList* GetSelectionConstraints() const;
	virtual Ids GetSelectedIds() const;
	virtual bool SetSelectedIds(const Ids& selectedIds);

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	Ids m_selectedIds;
};


class CPerson: virtual public istd::IChangeable
{
public:
	virtual QString GetName() const
	{
		return m_name;
	}

	// reimplemented (istd::IChangeable)

	virtual int GetSupportedOperations() const
	{
		// Tell to the world, which operation are supported by our implementation:
		return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
	}

	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS)
	{
		// Check if the input object is a person:
		const CPerson* sourcePtr = dynamic_cast<const CPerson*>(&object);
		if (sourcePtr != nullptr){
			// Check if the name differs from the source:
			if (m_name != sourcePtr->m_name){
				// Instantiate the change notifier. In the constructor it will call BeginChanges method:
				istd::CChangeNotifier changeNotifier(this);

				// Do copy:
				m_name = sourcePtr->m_name;

				// At this point the change notifier will be destroyed and call in the destructor the EndChanges method
			}

			return true;
		}

		return false;
	}

	virtual bool IsEqual(const IChangeable& object)
	{
		// Check if the input object is a person:
		const CPerson* sourcePtr = dynamic_cast<const CPerson*>(&object);
		if (sourcePtr != nullptr){
			// Check if the name differs from the source:
			return (m_name == sourcePtr->m_name);
		}

		return false;
	}

	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const
	{
		// Create a new instance of CPerson
		istd::TDelPtr<CPerson> clonePtr(new CPerson());

		// Try do copy:
		if (clonePtr->CopyFrom(*this, mode)){
			// if copy was successfull, release the object instance from the auto-pointer and return it:
			return clonePtr.PopPtr();
		}

		// At this point the allocated instance will be released from memory by destructor of TDelPtr!
		return nullptr;
	}

	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS)
	{
		if (!m_name.isEmpty()){
			istd::CChangeNotifier changeNotifier(this);

			m_name.clear();
		}

		return true;
	}

private:
	QString m_name;
};

} // namespace imtbase


