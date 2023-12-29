#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRequestExtractor: virtual public istd::IPolymorphic
{
public:

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeable* ExtractObject(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const = 0;
};


} // namespace imtgql


