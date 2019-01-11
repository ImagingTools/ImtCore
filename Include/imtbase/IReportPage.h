#pragma once


// Qt includes
#include <QtCore/QVector>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class IGraphicsElement;


/**
	Interface for a page in the report document.
*/
class IReportPage: virtual public iser::ISerializable
{
public:
	typedef QVector<QByteArray> ElementIds;

	/**
		Get the list of page elements.
	*/
	virtual ElementIds GetPageElements() const = 0;

	/**
		Get a page element with a given ID
	*/
	virtual const IGraphicsElement* GetPageElement(const QByteArray& elementId) const = 0;
};


} // namespace imtbase


