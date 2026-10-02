// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/IDatabaseAccessContextController.h>


namespace imtdb
{


/**
	Tenant access context of the request processed by the calling thread.
	The context is set by the request entry point (e.g. imtservergql::CHttpGraphQLServletComp)
	and read by the database engine. A thread without context gets \c AM_NONE.
*/
class CDatabaseAccessContextComp:
			public icomp::CComponentBase,
			virtual public IDatabaseAccessContextController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseAccessContextComp);
		I_REGISTER_INTERFACE(IDatabaseAccessContext);
		I_REGISTER_INTERFACE(IDatabaseAccessContextController);
	I_END_COMPONENT;

	// reimplemented (imtdb::IDatabaseAccessContext)
	virtual AccessMode GetAccessMode() const override;
	virtual QByteArray GetTenantId() const override;
	virtual QByteArray GetUserId() const override;

	// reimplemented (imtdb::IDatabaseAccessContextController)
	virtual void SetTenantAccessContext(const QByteArray& tenantId, const QByteArray& userId) override;
	virtual void ResetAccessContext() override;

private:
	struct TenantContext
	{
		QByteArray tenantId;
		QByteArray userId;
	};

	bool GetCurrentContext(TenantContext& context) const;

private:
	QHash<Qt::HANDLE, TenantContext> m_threadContexts;
	mutable QMutex m_threadContextsMutex;
};


} // namespace imtdb


