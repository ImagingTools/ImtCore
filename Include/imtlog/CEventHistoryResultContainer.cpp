// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CEventHistoryResultContainer.h>


namespace imtlog
{


// public methods

CEventHistoryResultContainer::CEventHistoryResultContainer()
	:m_isClosed(false)
{
}


void CEventHistoryResultContainer::Close()
{
	QMutexLocker locker(&m_mutex);
	m_isClosed = true;
}


bool CEventHistoryResultContainer::IsClosed()
{
	QMutexLocker locker(&m_mutex);
	return m_isClosed;
}


}


