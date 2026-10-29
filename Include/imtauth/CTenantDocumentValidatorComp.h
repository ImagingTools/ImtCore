// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// Qt includes
#include <QtCore/QByteArrayList>

// ImtCore includes
#include <imtdoc/IDocumentValidator.h>
#include <imtbase/IOperationContext.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{

bool IsTenantPermissionScopeChangeAllowed(
			const QByteArray& creatorId,
			const QByteArrayList& currentPermissions,
			const QByteArrayList& newPermissions,
			const QByteArray& userId);


/**
	Validates Tenant documents before save.
	Ensures tenant name is specified and unique.
*/
class CTenantDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDocumentValidatorComp)
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection used for uniqueness checks", true, "TenantCollection");
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const iprm::IParamsSet& params, const istd::IChangeable& document, QString& errorMessage, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauth
