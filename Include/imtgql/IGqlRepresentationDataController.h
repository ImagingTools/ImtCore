#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


/**
	Common intreface for getting data model representation based on a GraphQL-request.
*/
class IGqlRepresentationDataController: virtual public istd::IPolymorphic
{
public:
	/**
		Get ID of the supported data model.
	*/
	virtual QByteArrayList GetModelIds() const = 0;

	/**
		Get context IDs of the supported data model.
	*/
	virtual QByteArrayList GetContextIds() const = 0;

	/**
		Create representation data for a given GraphQL request.
	*/
	virtual imtbase::CHierarchicalItemModelPtr CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace imtgql


