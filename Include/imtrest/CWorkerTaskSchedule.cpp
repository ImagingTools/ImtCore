// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerTaskSchedule.h>


namespace imtrest
{


bool CWorkerTaskSchedule::Append(const QByteArray& orderingKey, Task task)
{
	if (m_isShuttingDown || !task){
		return false;
	}

	Item item;
	item.task = std::move(task);
	item.orderingKey = orderingKey;

	m_pendingList.append(std::move(item));

	return true;
}


bool CWorkerTaskSchedule::TakeDispatchable(Item& result)
{
	const int index = FindDispatchableIndex();
	if (index < 0){
		return false;
	}

	result = m_pendingList.takeAt(index);

	if (!result.orderingKey.isEmpty()){
		m_inFlightKeys.insert(result.orderingKey);
	}

	return true;
}


void CWorkerTaskSchedule::ReleaseKey(const QByteArray& orderingKey)
{
	if (!orderingKey.isEmpty()){
		m_inFlightKeys.remove(orderingKey);
	}
}


QList<CWorkerTaskSchedule::Item> CWorkerTaskSchedule::Shutdown()
{
	m_isShuttingDown = true;

	QList<Item> droppedList = m_pendingList;

	m_pendingList.clear();
	m_inFlightKeys.clear();

	return droppedList;
}


bool CWorkerTaskSchedule::IsShuttingDown() const
{
	return m_isShuttingDown;
}


int CWorkerTaskSchedule::GetPendingCount() const
{
	return m_pendingList.count();
}


bool CWorkerTaskSchedule::IsKeyInFlight(const QByteArray& orderingKey) const
{
	return m_inFlightKeys.contains(orderingKey);
}


// private methods

int CWorkerTaskSchedule::FindDispatchableIndex() const
{
	for (int index = 0; index < m_pendingList.count(); ++index){
		const Item& item = m_pendingList.at(index);

		if (item.orderingKey.isEmpty() || !m_inFlightKeys.contains(item.orderingKey)){
			return index;
		}
	}

	return -1;
}


} // namespace imtrest
