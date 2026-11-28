#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <icomp/CComponentBase.h>

// ControlsGallery includes
#include <controlsgallerygql/IDemoTaskStore.h>


namespace controlsgallerygql
{


/**
	In-memory task list of the assistant example; the tasks of a tenant are created on first use.
*/
class CDemoTaskStoreComp:
			public icomp::CComponentBase,
			virtual public IDemoTaskStore
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDemoTaskStoreComp);
		I_REGISTER_INTERFACE(IDemoTaskStore);
	I_END_COMPONENT;

	// reimplemented (controlsgallerygql::IDemoTaskStore)
	virtual Tasks GetTasks(const QByteArray& tenantId) const override;
	virtual bool GetTask(const QByteArray& tenantId, const QByteArray& taskId, Task& task) const override;
	virtual ChangeResult SetTaskStatus(
				const QByteArray& tenantId,
				const QByteArray& taskId,
				const QString& status,
				int expectedRevision,
				Task& changedTask) override;

private:
	Tasks& GetTenantTasks(const QByteArray& tenantId) const;

private:
	mutable QMutex m_mutex;
	mutable QMap<QByteArray, Tasks> m_tasks;
};


} // namespace controlsgallerygql


