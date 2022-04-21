#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QByteArray>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class ICollectionInfo;


/**
	Selection of the elements identified by an unique ID.
*/
class ISelection: virtual public iser::ISerializable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	static const QByteArray CN_SELECTION_MODE_CHANGED;
	static const QByteArray CN_SELECTION_CHANGED;
	static const QByteArray CN_CONSTRAINTS_CHANGED;

	/**
		Change notification flags.
	*/
	enum ChangeFlags
	{
		CF_SELECTION_MODE_CHANGED = 0x373978,
		CF_SELECTION_CHANGED,
		CF_CONSTRAINTS_CHANGED
	};

	enum SelectionMode
	{
		SM_SINGLE = 1,
		SM_MULTI
	};

	I_DECLARE_ENUM(SelectionMode, SM_SINGLE, SM_MULTI);

	/**
		Get the list of options defining possible selection range.
	*/
	virtual const ICollectionInfo* GetSelectionConstraints() const = 0;

	/**
		Get selection mode.
	*/
	virtual SelectionMode GetSelectionMode() const = 0;

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


