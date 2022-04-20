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
	Multiple selection of the elements identified by an unique ID.
*/
class IMultiSelection: virtual public iser::ISerializable
{
public:
	typedef QByteArray Id;
	typedef QVector<QByteArray> Ids;

	static const QByteArray CN_SELECTION_CHANGED;

	/**
		Change notification flags.
	*/
	enum ChangeFlags
	{
		CF_SELECTION_CHANGED = 0x373978
	};

	enum class SelectionMode
	{
		SM_MULTI_SELECTION = 0,
		SM_SINGLE_SELECTION
	};

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


