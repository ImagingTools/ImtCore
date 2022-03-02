#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariant>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for describing the contents of a collection.
	Assuming that elements of a collection can be identified by a unique ID,
	this interface provides access to the general properties of an element in such collection.
	\ingroup Collection
*/
class ICollectionInfoProvider: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	/**
		Type of the element information.
	*/
	enum ElementInfoType
	{
		/**
			Name of the element given as a QString.
		*/
		EIT_NAME,

		/**
			Human-readable description of the element given as a QString.
		*/
		EIT_DESCRIPTION,

		/**
			Enabled/Disabled status of the element as a boolean.
		*/
		EIT_ENABLED,

		/**
			Type-ID of the element given as a QByteArray.
		*/
		EIT_TYPE_ID,

		/**
			Human-readable name of the element type given as a QString.
		*/
		EIT_TYPE_NAME,

		/**
			Start value for user-defined info types.
		*/
		EIT_USER = 1000
	};

	/**
		Get information about a given element.
	*/
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const = 0;
};


} // namespace imtbase


