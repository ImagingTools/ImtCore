// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentValidator.h>
#include <imtauth/ITenantConnectionRequestManager.h>


namespace imtauth
{


/**
	Validates CrossOrgGrant documents before save.
	Ensures required fields (targetTenantId, roleIds) are present and the organizations are connected.
*/
class CCrossOrgGrantDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCrossOrgGrantDocumentValidatorComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
		I_ASSIGN(m_connectionManagerCompPtr, "TenantConnectionRequestManager", "Connection manager used to require a connection between the organizations", false, "TenantConnectionRequestManager");
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtauth::ITenantConnectionRequestManager, m_connectionManagerCompPtr);
};


} // namespace imtauth
