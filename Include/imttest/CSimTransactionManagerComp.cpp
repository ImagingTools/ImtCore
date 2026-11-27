// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CSimTransactionManagerComp.h>


namespace imttest
{


// reimplemented (imtbase::ITransactionManager)

bool CSimTransactionManagerComp::StartTransaction()
{
	++startedCount;

	return true;
}


bool CSimTransactionManagerComp::CancelTransaction()
{
	++cancelledCount;

	return true;
}


bool CSimTransactionManagerComp::EndTransaction()
{
	++endedCount;

	return true;
}


} // namespace imttest

