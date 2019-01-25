#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtreport
{


class ITableItem;


/**
	Interface for a table.
*/
class ITable: virtual public iser::ISerializable
{
public:
	/**
		Constructor
	*/
	explicit ITable(const int rows, const int columns) = 0;

	/**
		Get rows count
	*/
	virtual int GetRowCount() const;

	/**
		Get columns count
	*/
	virtual int GetColumnCount() const;

	/**
		Get item
	*/
	virtual ITableItem* GetItem(const int row, const int column) const;

	/**
		Set item
	*/
	virtual void SetItem(const int row, const int column, const ITableItem* item);
};


} // namespace imtreport


