#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// imtbase includes
#include <imtbase/IOperationContext.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IOperationContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::IOperationContext* CreateOperationContext(
				int operationType,
				const imtgql::CGqlRequest& gqlRequest = imtgql::CGqlRequest(),
				const QByteArray& objectId = QByteArray(),
				const istd::IChangeable* objectDataPtr = nullptr,
				const iprm::IParamsSet* paramsPtr = nullptr) = 0;
};


} // namespace imtgql


