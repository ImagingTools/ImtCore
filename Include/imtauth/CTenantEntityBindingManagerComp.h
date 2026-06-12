// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/ITenantEntityBinding.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantEntityBindingManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantEntityBindingManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantEntityBindingManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantEntityBindingManager);
		I_ASSIGN(m_bindingsCollectionCompPtr, "BindingsCollection", "SQL-backed TenantEntityBindings collection", true, "TenantEntityBindingsCollection");
		I_ASSIGN(m_bindingFactoryCompPtr, "BindingFactory", "Factory for ITenantEntityBinding instances", true, "TenantEntityBindingInfo");
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantEntityBindingManager)
	virtual bool AddBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) override;
	virtual bool RemoveBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) override;
	virtual bool RemoveAllBindingsForEntity(const QByteArray& entityType, const QByteArray& entityId) override;
	virtual bool RemoveAllBindingsForTenant(const QByteArray& tenantId, const QByteArray& entityType) override;
	virtual bool HasBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) const override;
	virtual QByteArrayList GetEntityIds(const QByteArray& tenantId, const QByteArray& entityType) const override;

private:
	QByteArrayList FindBindingIds(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) const;

	I_REF(imtbase::IObjectCollection, m_bindingsCollectionCompPtr);
	I_FACT(imtauth::ITenantEntityBinding, m_bindingFactoryCompPtr);
};


} // namespace imtauth
