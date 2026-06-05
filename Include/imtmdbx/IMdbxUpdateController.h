#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/ITransactionManager.h>


namespace imtmdbx
{


class IMdbxUpdateController: virtual public imtbase::ITransactionManager
{
public:
	virtual bool Update() = 0;
};


} // namespace imtmdbx


