// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ITransactionManager.h>


namespace imttest
{


/**
	Transaction manager without a database: counts the transactions, so a test can tell
	whether a card started, cancelled or ended one.
*/
class CSimTransactionManagerComp:
			public icomp::CComponentBase,
			virtual public imtbase::ITransactionManager
{
public:
	using BaseClass = icomp::CComponentBase;

	I_BEGIN_COMPONENT(CSimTransactionManagerComp);
		I_REGISTER_INTERFACE(imtbase::ITransactionManager);
	I_END_COMPONENT;

	// reimplemented (imtbase::ITransactionManager)
	virtual bool StartTransaction() override;
	virtual bool CancelTransaction() override;
	virtual bool EndTransaction() override;

	int startedCount = 0;
	int cancelledCount = 0;
	int endedCount = 0;
};


} // namespace imttest

