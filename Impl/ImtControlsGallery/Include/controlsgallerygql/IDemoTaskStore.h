#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace controlsgallerygql
{


/**
	In-memory task list of the assistant example. Every tenant has its own tasks.
*/
class IDemoTaskStore: virtual public istd::IPolymorphic
{
public:
	struct Task
	{
		QByteArray id;
		QString title;
		QString status;
		int revision = 1;
	};

	typedef QList<Task> Tasks;

	enum ChangeResult
	{
		CR_CHANGED,
		CR_NOT_FOUND,
		CR_STALE
	};

	virtual Tasks GetTasks(const QByteArray& tenantId) const = 0;
	virtual bool GetTask(const QByteArray& tenantId, const QByteArray& taskId, Task& task) const = 0;

	/**
		Change the status of a task if its revision still is \c expectedRevision.
	*/
	virtual ChangeResult SetTaskStatus(
				const QByteArray& tenantId,
				const QByteArray& taskId,
				const QString& status,
				int expectedRevision,
				Task& changedTask) = 0;
};


} // namespace controlsgallerygql


