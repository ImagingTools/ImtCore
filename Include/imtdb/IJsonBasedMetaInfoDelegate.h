#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
	Common interface for meta information delegate based on the JSON-representation of the created table meta-infos.
	The delegate converts the meta information to a well-defined JSON schema and vice versa.
*/
class IJsonBasedMetaInfoDelegate : virtual public istd::IPolymorphic
{
public:
	/**
		Create JSON-representation of the given meta information.
	*/
	virtual bool ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json) const = 0;

	/**
		Create meta information from its JSON-representation.
	*/
	virtual bool FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo) const = 0;
};


} // namespace imtdb


