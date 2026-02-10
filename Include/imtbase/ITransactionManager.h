// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ITransactionManager: virtual public istd::IChangeable
{
public:
	virtual bool StartTransaction() = 0;
	virtual bool CancelTransaction() = 0;
	virtual bool EndTransaction() = 0;
};


} // namespace imtbase


