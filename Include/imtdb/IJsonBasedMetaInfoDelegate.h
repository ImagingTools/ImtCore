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
		Create JSON-representation of the given meta informations.
	*/
	virtual QByteArray ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo) const = 0;

	/**
		Create meta information from its JSON-representation.
	*/
	virtual idoc::MetaInfoPtr FromJsonRepresentation(const QByteArray& data) const = 0;
};


} // namespace imtdb


