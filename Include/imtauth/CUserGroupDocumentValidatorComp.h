// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentValidator.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Validates User Group documents before save.
	Ensures group name is specified and unique within the same tenant.
*/
class CUserGroupDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupDocumentValidatorComp)
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User group collection used for uniqueness checks", true, "UserGroupCollection");
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	/**
		Check that no other group of the tenant has the same name.
		An empty tenant ID checks the groups of no organization.
	*/
	static bool CheckGroupNameUnique(
				const imtbase::IObjectCollection& userGroupCollection,
				const QByteArray& objectId,
				const QString& name,
				const QByteArray& tenantId,
				QString& errorMessage);

	/**
		Check that every parent group belongs to the tenant (or, for an empty tenant ID, to no organization).
	*/
	static bool CheckParentGroupsInTenant(
				const imtbase::IObjectCollection& userGroupCollection,
				const QByteArrayList& parentGroupIds,
				const QByteArray& tenantId,
				QString& errorMessage);

private:
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauth
