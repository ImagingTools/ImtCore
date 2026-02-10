// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskCollectionContextComp.h>


namespace imthype
{


// public methods

#if QT_VERSION >= 0x060000
CTaskCollectionContextComp::CTaskCollectionContextComp()
{
}
#else
CTaskCollectionContextComp::CTaskCollectionContextComp()
	: m_dataMutex(QMutex::Recursive)
{
}
#endif


// reimplemented (imthype::ITaskCollectionContext)

QByteArray CTaskCollectionContextComp::GetTaskId() const
{
	QMutexLocker lock(&m_dataMutex);

	return m_taskId;
}


void CTaskCollectionContextComp::SetTaskId(const QByteArray& taskId)
{
	QMutexLocker lock(&m_dataMutex);

	if (m_taskId != taskId){
		istd::CChangeNotifier changeNotifier(this);

		m_taskId = taskId;

		lock.unlock();
	}
}


QByteArray CTaskCollectionContextComp::GetTaskInputId() const
{
	QMutexLocker lock(&m_dataMutex);

	return m_taskInputId;
}


void CTaskCollectionContextComp::SetTaskInputId(const QByteArray& taskInputId)
{
	QMutexLocker lock(&m_dataMutex);

	if (m_taskInputId != taskInputId){
		istd::CChangeNotifier changeNotifier(this);

		m_taskInputId = taskInputId;

		lock.unlock();
	}
}


// reimplemented (iser::ISerializable)

bool CTaskCollectionContextComp::Serialize(iser::IArchive & archive)
{
	QMutexLocker lock(&m_dataMutex);

	istd::CChangeNotifier changePtr(archive.IsStoring() ? NULL : this);

	bool retVal = true;

	static iser::CArchiveTag taskInputIdTag("TaskInputId", "ID of the task input", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(taskInputIdTag);
	retVal = retVal && archive.Process(m_taskInputId);
	retVal = retVal && archive.EndTag(taskInputIdTag);

	static iser::CArchiveTag taskIdTag("TaskId", "ID of the task in the collection", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(taskIdTag);
	retVal = retVal && archive.Process(m_taskId);
	retVal = retVal && archive.EndTag(taskIdTag);

	lock.unlock();

	return retVal;
}


} // namespace imthype


