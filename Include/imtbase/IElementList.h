#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QSet>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class IElementList: virtual public istd::IChangeable
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
		Return \c true if the element is enabled and can be selected.
	*/
	virtual bool IsElementEnabled(const QByteArray& elementId) const = 0;
};


} // namespace imtbase


