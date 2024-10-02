#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// imtbase includes
#include <imtbase/IOperationContext.h>


namespace imtgql
{


class IOperationContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::IOperationContext* CreateOperationContext(
				const QByteArray& operationTypeId,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const iprm::IParamsSet* paramsPtr = nullptr) = 0;
};


} // namespace imtgql


