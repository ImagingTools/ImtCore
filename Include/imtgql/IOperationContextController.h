#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// imtbase includes
#include <imtbase/IOperationContext.h>
#include <imtbase/IOperationDescription.h>


namespace imtgql
{


class IOperationContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::IOperationContext* CreateOperationContext(
				imtbase::IOperationDescription::OperationType operationType,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const iprm::IParamsSet* paramsPtr = nullptr) = 0;
};


} // namespace imtgql


