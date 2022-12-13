#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ITransactionManager: virtual public istd::IChangeable
{
public:
	virtual bool StartTransaction() = 0;
	virtual bool EndTransaction() = 0;
};


} // namespace imtbase


