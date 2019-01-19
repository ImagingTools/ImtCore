#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <iser/ISerializable.h>
#include <i2d/CVector2d.h>

// ImtCore includes
#include <imtreport/IGraphicsElement.h>


namespace imtreport
{


class IGraphicsElement;


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
	virtual const IGraphicsElement* GetPageElement(const QByteArray& elementId) const = 0;

	/**
		Add a text label to the report page.
	*/
	virtual QByteArray AddText(const QString& text, const i2d::CVector2d& position) = 0;

	/**
		Add a text label to the report page.
	*/
	virtual QByteArray AddText(const QString& text, const double top, const IGraphicsElement::Alignment alignment) = 0;

	/**
		Add an image file to the report page.
	*/
	virtual QByteArray AddImage(const QString& imagePath, const QRect& rect) = 0;

	/**
		Add a rectangle to the report page.
	*/
	virtual QByteArray AddRectangle(const QRect& rect) = 0;

	/**
		Add a polygone to the report page.
	*/
	virtual QByteArray AddPolygone(const QVector<i2d::CVector2d>& points) = 0;

	/**
		Remove a page element.
	*/
	virtual bool RemovePageElement(const QByteArray& elementId) = 0;
};


} // namespace imtreport


