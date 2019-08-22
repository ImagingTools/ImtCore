#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QSet>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for describing the contents of a collection.
	Assuming that elements of a collection can be assigned by a unique ID,
	this interface provides access to the general properties of the elements in such a collection
*/
class ICollectionInfo: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QSet<Id> Ids;

	/**
		Get available element-IDs.
	*/
	virtual Ids GetElementIds() const = 0;

	/**
		Get name of the element with the given ID.
	*/
	virtual QString GetElementName(const QByteArray& elementId) const = 0;

	/**
		Get the human-readable description for an element with the given ID.
	*/
	virtual QString GetElementDescription(const QByteArray& elementId) const = 0;

	/**
		Return \c true if the element is enabled.
	*/
	virtual bool IsElementEnabled(const QByteArray& elementId) const = 0;
};


} // namespace imtbase


