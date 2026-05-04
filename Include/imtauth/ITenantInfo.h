// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	Interface for describing a tenant.
	\ingroup Tenant
*/
class ITenantInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_TENANT_ID = idoc::IDocumentMetaInfo::MIT_USER + 200,
		MIT_TENANT_NAME,
		MIT_TENANT_DESCRIPTION,
		MIT_TENANT_IS_ACTIVE
	};

	/**
		Get tenant ID.
	*/
	virtual QByteArray GetTenantId() const = 0;

	/**
		Set tenant ID.
	*/
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	/**
		Get tenant name.
	*/
	virtual QString GetTenantName() const = 0;

	/**
		Set tenant name.
	*/
	virtual void SetTenantName(const QString& name) = 0;

	/**
		Get tenant description.
	*/
	virtual QString GetTenantDescription() const = 0;

	/**
		Set tenant description.
	*/
	virtual void SetTenantDescription(const QString& description) = 0;

	/**
		Check if tenant is active.
	*/
	virtual bool IsActive() const = 0;

	/**
		Set active state.
	*/
	virtual void SetActive(bool isActive) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantInfo> ITenantInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantInfo> ITenantInfoSharedPtr;


} // namespace imtauth


