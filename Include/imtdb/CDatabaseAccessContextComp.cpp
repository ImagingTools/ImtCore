// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDatabaseAccessContextComp.h>


// Qt includes
#include <QtCore/QThread>


namespace imtdb
{


// reimplemented (imtdb::IDatabaseAccessContext)

IDatabaseAccessContext::AccessMode CDatabaseAccessContextComp::GetAccessMode() const
{
	TenantContext context;

	return GetCurrentContext(context) ? AM_TENANT : AM_NONE;
}


QByteArray CDatabaseAccessContextComp::GetTenantId() const
{
	TenantContext context;
	GetCurrentContext(context);

	return context.tenantId;
}


QByteArray CDatabaseAccessContextComp::GetUserId() const
{
	TenantContext context;
	GetCurrentContext(context);

	return context.userId;
}


// reimplemented (imtdb::IDatabaseAccessContextController)

void CDatabaseAccessContextComp::SetTenantAccessContext(const QByteArray& tenantId, const QByteArray& userId)
{
	QMutexLocker locker(&m_threadContextsMutex);

	TenantContext& context = m_threadContexts[QThread::currentThreadId()];
	context.tenantId = tenantId;
	context.userId = userId;
}


void CDatabaseAccessContextComp::ResetAccessContext()
{
	QMutexLocker locker(&m_threadContextsMutex);

	m_threadContexts.remove(QThread::currentThreadId());
}


// private methods

bool CDatabaseAccessContextComp::GetCurrentContext(TenantContext& context) const
{
	QMutexLocker locker(&m_threadContextsMutex);

	QHash<Qt::HANDLE, TenantContext>::const_iterator iter = m_threadContexts.constFind(QThread::currentThreadId());
	if (iter == m_threadContexts.cend()){
		return false;
	}

	context = *iter;

	return true;
}


} // namespace imtdb


