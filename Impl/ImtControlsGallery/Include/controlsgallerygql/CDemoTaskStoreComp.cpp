#include <controlsgallerygql/CDemoTaskStoreComp.h>


// Qt includes
#include <QtCore/QMutexLocker>


namespace controlsgallerygql
{


// reimplemented (controlsgallerygql::IDemoTaskStore)

IDemoTaskStore::Tasks CDemoTaskStoreComp::GetTasks(const QByteArray& tenantId) const
{
	QMutexLocker lock(&m_mutex);

	return GetTenantTasks(tenantId);
}


bool CDemoTaskStoreComp::GetTask(const QByteArray& tenantId, const QByteArray& taskId, Task& task) const
{
	QMutexLocker lock(&m_mutex);

	for (const Task& existing: GetTenantTasks(tenantId)){
		if (existing.id == taskId){
			task = existing;

			return true;
		}
	}

	return false;
}


IDemoTaskStore::ChangeResult CDemoTaskStoreComp::SetTaskStatus(
			const QByteArray& tenantId,
			const QByteArray& taskId,
			const QString& status,
			int expectedRevision,
			Task& changedTask)
{
	QMutexLocker lock(&m_mutex);

	for (Task& existing: GetTenantTasks(tenantId)){
		if (existing.id != taskId){
			continue;
		}

		if (existing.revision != expectedRevision){
			return CR_STALE;
		}

		existing.status = status;
		++existing.revision;
		changedTask = existing;

		return CR_CHANGED;
	}

	return CR_NOT_FOUND;
}


// private methods

IDemoTaskStore::Tasks& CDemoTaskStoreComp::GetTenantTasks(const QByteArray& tenantId) const
{
	QMap<QByteArray, Tasks>::iterator iter = m_tasks.find(tenantId);
	if (iter != m_tasks.end()){
		return iter.value();
	}

	static const char* const titles[] = {
		"Prepare release notes",
		"Review pull request",
		"Update installation guide",
		"Answer customer request",
		"Plan sprint review"
	};
	static const char* const statuses[] = {"open", "open", "inProgress", "open", "done"};

	Tasks tasks;
	for (int i = 0; i < 5; ++i){
		Task task;
		task.id = QByteArray("task-") + QByteArray::number(i + 1);
		task.title = QString::fromLatin1(titles[i]);
		task.status = QString::fromLatin1(statuses[i]);
		tasks.append(task);
	}

	return m_tasks.insert(tenantId, tasks).value();
}


} // namespace controlsgallerygql


