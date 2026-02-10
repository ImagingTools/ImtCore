// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/CMessageContainer.h>


namespace imtlog
{


class CEventHistoryResultContainer:
			public imod::TModelWrap<ilog::CMessageContainer>
{
public:
	CEventHistoryResultContainer();

	void Close();
	bool IsClosed();

private:
	bool m_isClosed;
	QMutex m_mutex;
};


} // namespace imtlog


