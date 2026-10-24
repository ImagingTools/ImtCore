// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtdoc/IDocumentValidator.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Validates Role documents before save.
	Ensures role-ID is specified and unique within the same tenant and product.
*/
class CRoleDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRoleDocumentValidatorComp)
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection used for uniqueness checks", true, "RoleCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager used for tenant-scoped permissions validation", false, "TenantManager");
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	/**
		Check that no other role of the tenant has the same role-ID within the product.
		An empty tenant ID checks the roles of no organization.
	*/
	static bool CheckRoleIdUnique(
				const imtbase::IObjectCollection& roleCollection,
				const QByteArray& objectId,
				const QByteArray& roleId,
				const QByteArray& productId,
				const QByteArray& tenantId,
				QString& errorMessage);

	/**
		Check that every parent role belongs to the tenant (or, for an empty tenant ID, to no organization).
	*/
	static bool CheckParentRolesInTenant(
				const imtbase::IObjectCollection& roleCollection,
				const QByteArrayList& parentRoleIds,
				const QByteArray& tenantId,
				QString& errorMessage);

private:
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth
