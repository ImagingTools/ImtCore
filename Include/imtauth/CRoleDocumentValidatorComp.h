// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtdoc/IDocumentValidator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TenantType.h>


namespace imtauth
{


/**
	Validates Role documents before save.
	Ensures role-ID is specified and unique within the same tenant and product.
	The messages are in the language of the caller (\c imtdoc::GetLanguageIdFromParams) when a translation manager is set.
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
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translates the validation messages into the language of the caller", false, "TranslationManager");
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const istd::IChangeable& document, QString& errorMessage, const iprm::IParamsSet* validationParams = nullptr, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	/**
		Check that no other role of the tenant has the same role-ID within the product.
		An empty tenant ID checks the roles of no organization.
		The message is translated into \a languageId when \a translationManagerPtr is set.
	*/
	static bool CheckRoleIdUnique(
				const imtbase::IObjectCollection& roleCollection,
				const QByteArray& objectId,
				const QByteArray& roleId,
				const QByteArray& productId,
				const QByteArray& tenantId,
				imtbase::TenantType tenantType,
				QString& errorMessage,
				const iqt::ITranslationManager* translationManagerPtr = nullptr,
				const QByteArray& languageId = QByteArray());

	/**
		Check that every parent role belongs to the tenant (or, for an empty tenant ID, to no organization).
	*/
	static bool CheckParentRolesInTenant(
				const imtbase::IObjectCollection& roleCollection,
				const QByteArrayList& parentRoleIds,
				const QByteArray& tenantId,
				imtbase::TenantType tenantType,
				QString& errorMessage,
				const iqt::ITranslationManager* translationManagerPtr = nullptr,
				const QByteArray& languageId = QByteArray());

	/**
		Check that the parent roles belong to the product of the role.
	*/
	static bool CheckParentRolesInProduct(
				const imtbase::IObjectCollection& roleCollection,
				const QByteArrayList& parentRoleIds,
				const QByteArray& productId,
				QString& errorMessage,
				const iqt::ITranslationManager* translationManagerPtr = nullptr,
				const QByteArray& languageId = QByteArray());

private:
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtauth
