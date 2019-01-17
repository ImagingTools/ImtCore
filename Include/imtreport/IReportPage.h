#pragma once


// Qt includes
#include <QtCore>

// ACF includes
#include <iser/ISerializable.h>
#include <i2d/CVector2d.h>

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
		Get a page element with a given ID
	*/
	virtual const IGraphicsElement* GetPageElement(const QByteArray& elementId) const = 0;

	/**
		Add a new page element
	*/
	virtual QByteArray AddText(const QString& text, const i2d::CVector2d& position) = 0;
	
	/**
		Add an image file to the report page.
	*/
	virtual QByteArray AddImage(const QString& imagePath, const i2d::CVector2d& position) = 0;

	/**
		Remove a page element
	*/
	virtual bool RemovePageElement(const QByteArray& elementId) = 0;
};


} // namespace imtreport


