#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QByteArray>

// ACF includes
#include <iser/ISerializable.h>


namespace iprm
{
	class IOptionsList;
}


namespace imtbase
{


class IMultiSelection: virtual public iser::ISerializable
{
public:
	typedef QSet<QByteArray> Ids;

	/**
		Change notification flags.
	*/
	enum ChangeFlags
	{
		CF_SELECTION_CHANGED = 0x373978
	};

	/**
		Get the list of options defining possible selection range.
	*/
	virtual const iprm::IOptionsList* GetSelectionConstraints() const = 0;

	/**
		Get selected IDs.
	*/
	virtual Ids GetSelectedIds() const = 0;

	/**
		Set selected IDs.
	*/
	virtual bool SetSelectedIds(const Ids& selectedIds) = 0;
};


} // namespace imtbase


