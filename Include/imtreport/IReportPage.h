#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QColor>

// ACF includes
#include <iser/ISerializable.h>
#include <i2d/CVector2d.h>
#include <i2d/CLine2d.h>


namespace imtreport
{


class IObject2d;


/**
	Interface for a page in the report document.
*/

class IReportPage: virtual public iser::ISerializable
{
public:
	typedef QList<QByteArray> ElementIds;

	/**
		Get the list of page elements.
	*/
	virtual ElementIds GetPageElements() const = 0;

	/**
		Get a page element with a given ID.
	*/
	virtual i2d::IObject2d* GetPageElement(const QByteArray& elementId) const = 0;

	/**
		Add a text label to the report page.
	*/
	virtual QByteArray AddText(
				const QString& text,
				const i2d::CVector2d& position,
				const double textWidth = -1.0,
				const Qt::Alignment alignment = Qt::AlignLeft) = 0;

	/**
		Add an image file to the report page.
	*/
	virtual QByteArray AddImage(const QString& imagePath, const i2d::CRectangle &rect) = 0;

	/**
		Add a line to the report page.
	*/
	virtual QByteArray AddLine(const i2d::CLine2d& line) = 0;

	/**
		Add a rectangle to the report page.
	*/
	virtual QByteArray AddRectangle(const i2d::CRectangle& rect, const QColor& fillColor = Qt::transparent) = 0;

	/**
		Add a polygone to the report page.
	*/
	virtual QByteArray AddPolygone(const QVector<i2d::CVector2d>& points, const QColor& fillColor = Qt::transparent) = 0;

	/**
		Add a text table to the report page.
	*/
	virtual QByteArray AddTextTable(int rowCount, int columnCount, const i2d::CVector2d& topLeft) = 0;

	/**
		Remove a page element.
	*/
	virtual bool RemovePageElement(const QByteArray& elementId) = 0;
};


} // namespace imtreport


