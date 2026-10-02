// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IAccessContextController.h>


namespace imtbase
{


/**
	Tenant access context of the request processed by the calling thread.
	The context is set by the request entry point (e.g. imtservergql::CHttpGraphQLServletComp)
	and read by the database engine and the caches. A thread without context gets \c AM_NONE.
*/
class CAccessContextComp:
			public icomp::CComponentBase,
			virtual public IAccessContextController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAccessContextComp);
		I_REGISTER_INTERFACE(IAccessContext);
		I_REGISTER_INTERFACE(IAccessContextController);
	I_END_COMPONENT;

	// reimplemented (imtbase::IAccessContext)
	virtual AccessMode GetAccessMode() const override;
	virtual QByteArray GetTenantId() const override;
	virtual QByteArray GetUserId() const override;

	// reimplemented (imtbase::IAccessContextController)
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


} // namespace imtbase


