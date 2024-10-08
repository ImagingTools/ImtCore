#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
	Common interface for a SQL-table delegate.
	The delegate is responsible for description of a table in the SQL database.
*/
class IJsonBasedMetaInfoDelegate : virtual public istd::IPolymorphic
{
public:
	virtual QByteArray ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo) const = 0;
	virtual idoc::MetaInfoPtr FromJsonRepresentation(const QByteArray& data) const = 0;
};


} // namespace imtdb


