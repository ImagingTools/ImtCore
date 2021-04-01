#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QByteArray;
class QVariant;


namespace imtbase
{


class IObjectCollection;


}


namespace imtbase
{


class IObjectMetaInfoUniquenessValidator: virtual public istd::IPolymorphic
{
public:
	enum ResultFlags
	{
		/**
			Value is unique.
		*/
		RF_IS_UNIQUE = 0x00,

		/**
			Value not unique.
		*/
		RF_NOT_UNIQUE = 0x01,

		/**
			Found invalid collection reference
		*/
		RF_INVALID_COLLECTION_REFERENCE = 0x02,

		/**
			Missing object data metainfo.
		*/
		RF_MISSING_METAINFO = 0x04,

		/**
			Missing type in the object data metainfo.
		*/
		RF_MISSING_METAINFO_TYPE = 0x08,

		/**
			Found value with incompatible QVariant type
		*/
		RF_INCOMPATIBLE_VALUE_TYPE = 0x10
	};

	/**
		Check value uniqueness
		The combination of collectionPtr and objectId is excluded from validation.
	*/
	virtual int IsUniqueValue(const imtbase::IObjectCollection* collectionPtr, const QByteArray& objectId, const QVariant& value) const = 0;
};


} // namespace imtbase


