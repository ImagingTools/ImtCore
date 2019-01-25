#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtreport
{


/**
	Interface for a table item.
*/
class ITableItem: virtual public iser::ISerializable
{
public:
	/**
		Get item row
	*/
	virtual int GetRow() const = 0;

	/**
		Get item column
	*/
	virtual int GetColumn() const = 0;

	/**
		Get item text
	*/
	virtual QString GetText() const = 0;

	/**
		Set item text
	*/
	virtual void SetText(const QString& text) = 0;

	/**
		Get item text alignment
	*/
	virtual Qt::Alignment GetAlignment() const = 0;

	/**
		Set item text alignment
	*/
	virtual void SetAlignment(const Qt::Alignment alignment) = 0;

	/**
		Get item font size
	*/
	virtual double GetFontSize() const = 0;

	/**
		Set item font size
	*/
	virtual void SetFontSize(const double fontSize) = 0;

	/**
		Get item font name
	*/
	virtual QString GetFontName() const = 0;

	/**
		Set item font name
	*/
	virtual void SetFontName(const QString& fontName) = 0;

	/**
		Get item foreground color
	*/
	virtual QColor GetForegroundColor() const = 0;

	/**
		Set item foreground color
	*/
	virtual void SetForegroundColor(const QColor& color) = 0;

	/**
		Get item background color
	*/
	virtual QColor GetBackgroundColor() const = 0;

	/**
		Set item background color
	*/
	virtual void SetBackgroundColor(const QColor& color) = 0;
};


} // namespace imtreport


