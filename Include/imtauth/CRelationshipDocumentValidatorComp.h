// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtauth
{


/**
	Validates TenantRelationship documents before save.
	Ensures required fields (sourceTenantId, targetTenantId) are present
	and prevents duplicate relationships (same tenant pair with same role combination).
*/
class CRelationshipDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRelationshipDocumentValidatorComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
		I_ASSIGN(m_relationshipCollectionCompPtr, "RelationshipCollection", "Tenant relationships collection for duplicate check", false, "RelationshipCollection");
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_relationshipCollectionCompPtr);
};


} // namespace imtauth
