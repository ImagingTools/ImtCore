#pragma once


// ImtCore includes
#include <imtbase/IMultiSelection.h>


namespace imtbase
{


class CMultiSelection: virtual public IMultiSelection
{
public:
	// reimplemented (IMultiSelection)
	virtual const iprm::IOptionsList* GetSelectionConstraints() const;
	virtual Ids GetSelectedIds() const;
	virtual bool SetSelectedIds(const Ids& selectedIds);

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);
private:
	Ids m_selectedIds;
};


} // namespace imtbase


